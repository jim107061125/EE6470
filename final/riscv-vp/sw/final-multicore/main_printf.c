
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "stdlib.h"
#include "stdint.h"
#include "cassert"
//#include <cstdlib>
//#include <iostream>
//#include <fstream>

int sem_init (uint32_t *__sem, uint32_t count) __THROW
{
  *__sem=count;
  return 0;
}

int sem_wait (uint32_t *__sem) __THROW
{
  uint32_t value, success; //RV32A
  __asm__ __volatile__("\
L%=:\n\t\
     lr.w %[value],(%[__sem])            # load reserved\n\t\
     beqz %[value],L%=                   # if zero, try again\n\t\
     addi %[value],%[value],-1           # value --\n\t\
     sc.w %[success],%[value],(%[__sem]) # store conditionally\n\t\
     bnez %[success], L%=                # if the store failed, try again\n\t\
"
    : [value] "=r"(value), [success]"=r"(success)
    : [__sem] "r"(__sem)
    : "memory");
  return 0;
}

int sem_post (uint32_t *__sem) __THROW
{
  uint32_t value, success; //RV32A
  __asm__ __volatile__("\
L%=:\n\t\
     lr.w %[value],(%[__sem])            # load reserved\n\t\
     addi %[value],%[value], 1           # value ++\n\t\
     sc.w %[success],%[value],(%[__sem]) # store conditionally\n\t\
     bnez %[success], L%=                # if the store failed, try again\n\t\
"
    : [value] "=r"(value), [success]"=r"(success)
    : [__sem] "r"(__sem)
    : "memory");
  return 0;
}

int barrier(uint32_t *__sem, uint32_t *__lock, uint32_t *counter, uint32_t thread_count) {
	sem_wait(__lock);
	if (*counter == thread_count - 1) { //all finished
		*counter = 0;
		sem_post(__lock);
		for (int j = 0; j < thread_count - 1; ++j) sem_post(__sem);
	} else {
		(*counter)++;
		sem_post(__lock);
		sem_wait(__sem);
	}
	return 0;
}

//Convert a float into a string
//Only 4 digits of fractional parts are converted
//If more digits are needed, please modify the tmpInt2 scaling
void sprintfloat(char* buf, float num) {
	char temp1[] = "-";
	char temp2[] = "";
	char* tmpSign = (num < 0) ? temp1 : temp2;
	float tmpVal = (num < 0) ? -num : num;

	int tmpInt1 = (int) tmpVal;                  // Get the integer (678).
	float tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
	int tmpInt2 = trunc(tmpFrac * 10000);  // Turn into integer (123).

	// Print as parts, note that you need 0-padding for fractional bit.
	sprintf(buf, "%s%d.%04d", tmpSign, tmpInt1, tmpInt2);
}

//Total number of cores
//static const int PROCESSORS = 2;
#define PROCESSORS 2
//the barrier synchronization objects
uint32_t barrier_counter=0; 
uint32_t barrier_lock; 
uint32_t barrier_sem; 
//the mutex object to control global summation
uint32_t lock;  
//print synchronication semaphore (print in core order)
uint32_t print_sem[PROCESSORS]; 
//global summation variable
float pi_over_4 = 0;

union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

// Sobel Filter ACC
static char* const SOBELFILTER_START_ADDR = reinterpret_cast<char* const>(0x73000000);
static char* const SOBELFILTER_A_ADDR = reinterpret_cast<char* const>(0x73000004);
static char* const SOBELFILTER_B_ADDR = reinterpret_cast<char* const>(0x73000008);
static char* const SOBELFILTER_READ_ADDR  = reinterpret_cast<char* const>(0x7300000C);
static char* const SOBELFILTER_DONE_ADDR  = reinterpret_cast<char* const>(0x73000010);

// DMA 
static volatile uint32_t * const DMA_SRC_ADDR  = (uint32_t * const)0x70000000;
static volatile uint32_t * const DMA_DST_ADDR  = (uint32_t * const)0x70000004;
static volatile uint32_t * const DMA_LEN_ADDR  = (uint32_t * const)0x70000008;
static volatile uint32_t * const DMA_OP_ADDR   = (uint32_t * const)0x7000000C;
static volatile uint32_t * const DMA_STAT_ADDR = (uint32_t * const)0x70000010;
static const uint32_t DMA_OP_MEMCPY = 1;

bool _is_using_dma = true;
int A[8][8];
void write_data_to_ACC(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){  
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(buffer);
    *DMA_DST_ADDR = (uint32_t)(ADDR);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
  }else{
    // Directly Send
    memcpy(ADDR, buffer, sizeof(unsigned char)*len);
  }
}
void read_data_from_ACC(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(ADDR);
    *DMA_DST_ADDR = (uint32_t)(buffer);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
  }else{
    // Directly Read
    memcpy(buffer, ADDR, sizeof(unsigned char)*len);
  }
}

int main(unsigned hart_id) {

	/////////////////////////////
	// thread and barrier init //
	/////////////////////////////
	
	if (hart_id == 0) {
		// create a barrier object with a count of PROCESSORS
		sem_init(&barrier_lock, 1);
		sem_init(&barrier_sem, 0); //lock all cores initially
		for(int i=0; i< PROCESSORS; ++i){
			sem_init(&print_sem[i], 0); //lock printing initially
		}
		// Create mutex lock
		sem_init(&lock, 1);
	}

  	int i, j, x, y;        // for loop counter
	float r[8];
	//int A[8][8];
	int b[8];
  	float result;
  	int n = 8;
	int done = 0;
	char print[256];

  	int tmp;
  	word data;
	unsigned char buffer[4];

	int total_terms = n*n;
	int nc = 2;
	int num_terms_per_thread = (total_terms + nc - 1) / nc;
	int begin_idx = num_terms_per_thread * hart_id;
	int end_idx;

	if (hart_id == 1) {
		end_idx = total_terms;
	} else {
		end_idx = begin_idx + num_terms_per_thread;
	}
	sem_wait(&lock);
	for (int i = begin_idx; i < end_idx; i++) {
		if(hart_id == 0){
			A[i/n][i%n] = rand() % 20 +1;
		}else{
			A[i/n][i%n] = rand() % 30 +1;
		}
	}
	//sem_wait(&lock);
	sem_post(&lock);
	barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);
	//barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);
	/*for (y = 0; y != n; ++y) {
    	for (x = 0; x != n; ++x) {
			printf("%d\n%d\n", hart_id,A[y][x]);
    	}
  	}*/

	if (hart_id == 0) { 
		//printf("this %d\n", n);
		data.sint = n;
		buffer[0] = data.uc[0];
		buffer[1] = data.uc[1];
		buffer[2] = data.uc[2];
		buffer[3] = data.uc[3];
	}
	if (hart_id == 0) {
  		write_data_to_ACC(SOBELFILTER_START_ADDR, buffer, 4);
	}

	if (hart_id == 0) {
  		for (y = 0; y != n; ++y) {
    		for (x = 0; x != n; ++x) {
      		//input_file >> tmp;
			//tmp = rand() % 20 +1;
			//A[y][x] = tmp; 
			//tmp = tmp * (1 << 6);
			tmp = A[y][x] * (1 << 6);
      		data.sint = tmp;
			//printf("%d\n%d\n", x, data.sint);
			buffer[0] = data.uc[0];
			buffer[1] = data.uc[1];
			buffer[2] = data.uc[2];
			buffer[3] = data.uc[3];
      		write_data_to_ACC(SOBELFILTER_A_ADDR, buffer, 4);
    		}
  		}
	}

	if (hart_id == 0) {
  		for (i = 0; i != n; ++i) {
    		//input_file >> tmp;
			tmp = rand() % 20 + 1;
			b[i] = tmp;
			//printf("%d\n", b[i]);
			tmp = tmp * (1 << 10);
    		data.sint = tmp;
			buffer[0] = data.uc[0];
			buffer[1] = data.uc[1];
			buffer[2] = data.uc[2];
			buffer[3] = data.uc[3];
    		write_data_to_ACC(SOBELFILTER_B_ADDR, buffer, 4);
  		}
	}
  	//input_file.close();
	//fclose(fp);
    if(hart_id == 0){
        read_data_from_ACC(SOBELFILTER_DONE_ADDR, buffer, 4);
        data.uc[0] = buffer[0];
	    data.uc[1] = buffer[1];
	    data.uc[2] = buffer[2];
	    data.uc[3] = buffer[3];
        done = data.sint; 
        printf("%d\n", done);
    }

	if(hart_id == 0){
        read_data_from_ACC(SOBELFILTER_DONE_ADDR, buffer, 4);
        data.uc[0] = buffer[0];
	    data.uc[1] = buffer[1];
	    data.uc[2] = buffer[2];
	    data.uc[3] = buffer[3];
        done = data.sint; 
        printf("%d\n", done);
    }
    //barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);

	if ( done != 0 && hart_id == 0) { 		
  		for (j = 0; j != n; ++j){
    		read_data_from_ACC(SOBELFILTER_READ_ADDR, buffer, 4);
			data.uc[0] = buffer[0];
			data.uc[1] = buffer[1];
			data.uc[2] = buffer[2];
			data.uc[3] = buffer[3];
    		result = data.sint;
			r[j] = result / (1 << 10);
			sprintfloat(print, r[j]);
			printf("answer = %s\n", print);
            printf("%d\n", A[0][j]);
			printf("%d\n", b[j]); 
		  }   		
	}

	return 0;
}
