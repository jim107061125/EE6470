
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "stdlib.h"
#include "stdint.h"
#include "cassert"

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
//static char* const SOBELFILTER_START_ADDR = reinterpret_cast<char* const>(0x73000000);
//static char* const SOBELFILTER_A_ADDR = reinterpret_cast<char* const>(0x73000004);
//static char* const SOBELFILTER_B_ADDR = reinterpret_cast<char* const>(0x73000008);
//static char* const SOBELFILTER_READ_ADDR  = reinterpret_cast<char* const>(0x7300000C);
//static char* const SOBELFILTER_DONE_ADDR  = reinterpret_cast<char* const>(0x73000010);
static char* const CORE0_W1_ADDR = reinterpret_cast<char* const>(0x73000000);
static char* const CORE0_W2_ADDR = reinterpret_cast<char* const>(0x73000004);
static char* const CORE0_R_ADDR = reinterpret_cast<char* const>(0x73000010);
static char* const CORE1_W1_ADDR = reinterpret_cast<char* const>(0x74000000);
static char* const CORE1_W2_ADDR = reinterpret_cast<char* const>(0x74000004);
static char* const CORE1_R_ADDR = reinterpret_cast<char* const>(0x74000010);
static char* const PARA3_W1_ADDR = reinterpret_cast<char* const>(0x75000000);
static char* const PARA3_W2_ADDR = reinterpret_cast<char* const>(0x75000004);
static char* const PARA3_R_ADDR = reinterpret_cast<char* const>(0x75000010);
static char* const PARA4_W1_ADDR = reinterpret_cast<char* const>(0x76000000);
static char* const PARA4_W2_ADDR = reinterpret_cast<char* const>(0x76000004);
static char* const PARA4_R_ADDR = reinterpret_cast<char* const>(0x76000010);

// DMA 
static volatile uint32_t * const DMA_SRC_ADDR  = (uint32_t * const)0x70000000;
static volatile uint32_t * const DMA_DST_ADDR  = (uint32_t * const)0x70000004;
static volatile uint32_t * const DMA_LEN_ADDR  = (uint32_t * const)0x70000008;
static volatile uint32_t * const DMA_OP_ADDR   = (uint32_t * const)0x7000000C;
static volatile uint32_t * const DMA_STAT_ADDR = (uint32_t * const)0x70000010;
static const uint32_t DMA_OP_MEMCPY = 1;

bool _is_using_dma = true;
int A[4][4];
int A_ori[4][4];
int x[4];
int y[4];
int b[4];
int b_ori[4];
char* addr_w1;
char* addr_w2;
char* addr_r;
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

  	int i, j;        // for loop counter
	float r[4];
  	float result;
  	int n = 4;
	int done = 0;
	char print[256];

  	int tmp;
  	word data;
	word data2;
	word data3;
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
			A_ori[i/n][i%n] = A[i/n][i%n];
			A[i/n][i%n] = A[i/n][i%n] << 6;
		}else{
			A[i/n][i%n] = rand() % 30 +1;
			A_ori[i/n][i%n] = A[i/n][i%n];
			A[i/n][i%n] = A[i/n][i%n] << 6;
		}
	}
	//sem_wait(&lock);
	sem_post(&lock);
	//barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);
	//barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);
	/*if(hart_id == 0){
		for (int y = 0; y != n; ++y) {
    		for (int x = 0; x != n; ++x) {
				printf("%d\n",A_ori[y][x]);
    		}
  		}
	}*/
	barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);

	if(hart_id == 0){
		for (int j = 1; j < n; ++j) {
        	A[j][i] = A[j][i] << 6;
        	A[j][i] /= A[i][i];
			//printf("i=%d \n", i);
      	}
	}
	barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);

	int total_terms3 = 9;
	int num_terms_per_thread3 = (total_terms3 + nc - 1) / nc;
	int begin_idx3 = num_terms_per_thread3 * hart_id;
	int end_idx3;

	if (hart_id == 1) {
		end_idx3 = total_terms3;
	} else {
		end_idx3 = begin_idx3 + num_terms_per_thread3;
	}

	for (int i = begin_idx3; i < end_idx3; i=i+2) {
		int r1 = i / 3 + 1;
		int c1 = i % 3 + 1;
		int r2 = (i+1) / 3 + 1;
		int c2 = (i+1) % 3 + 1;
		if(hart_id == 0){
			data.sint = A[r1][0];
			buffer[0] = data.uc[0];
			buffer[1] = data.uc[1];
			buffer[2] = data.uc[2];
			buffer[3] = data.uc[3];
			write_data_to_ACC(CORE0_W1_ADDR, buffer, 4);
			//printf("A[j][i]=%d \n", A[j][i]);
			data2.sint = A[0][c1];
			buffer[0] = data2.uc[0];
			buffer[1] = data2.uc[1];
			buffer[2] = data2.uc[2];
			buffer[3] = data2.uc[3];
			write_data_to_ACC(CORE0_W2_ADDR, buffer, 4);
			//printf("A[i][k]=%d \n", A[i][k]);
			if((i+1) < end_idx3){
				data.sint = A[r2][0];
				buffer[0] = data.uc[0];
				buffer[1] = data.uc[1];
				buffer[2] = data.uc[2];
				buffer[3] = data.uc[3];
				write_data_to_ACC(PARA3_W1_ADDR, buffer, 4);
				//printf("A[j][i]=%d \n", A[j][i]);
				data2.sint = A[0][c2];
				buffer[0] = data2.uc[0];
				buffer[1] = data2.uc[1];
				buffer[2] = data2.uc[2];
				buffer[3] = data2.uc[3];
				write_data_to_ACC(PARA3_W2_ADDR, buffer, 4);
			}
			sem_post(&lock);
		}
		else{
			sem_wait(&lock);
			data.sint = A[r1][0];
			buffer[0] = data.uc[0];
			buffer[1] = data.uc[1];
			buffer[2] = data.uc[2];
			buffer[3] = data.uc[3];
			write_data_to_ACC(CORE1_W1_ADDR, buffer, 4);
			data2.sint = A[0][c1];
			buffer[0] = data2.uc[0];
			buffer[1] = data2.uc[1];
			buffer[2] = data2.uc[2];
			buffer[3] = data2.uc[3];
			write_data_to_ACC(CORE1_W2_ADDR, buffer, 4);
			//printf("A[i][k]=%d \n", A[i][k]);
			if((i+1) < end_idx3){
				data.sint = A[r2][0];
				buffer[0] = data.uc[0];
				buffer[1] = data.uc[1];
				buffer[2] = data.uc[2];
				buffer[3] = data.uc[3];
				write_data_to_ACC(PARA4_W1_ADDR, buffer, 4);
				//printf("A[j][i]=%d \n", A[j][i]);
				data2.sint = A[0][c2];
				buffer[0] = data2.uc[0];
				buffer[1] = data2.uc[1];
				buffer[2] = data2.uc[2];
				buffer[3] = data2.uc[3];
				write_data_to_ACC(PARA4_W2_ADDR, buffer, 4);
			}
		}

		if(hart_id == 0){
			read_data_from_ACC(CORE0_R_ADDR, buffer, 4);
			data3.uc[0] = buffer[0];
			data3.uc[1] = buffer[1];
			data3.uc[2] = buffer[2];
			data3.uc[3] = buffer[3];
    		tmp = data3.sint;
			A[r1][c1] -= tmp;
			//printf("tmp=%d \n", tmp);
			if((i+1) < end_idx3){
				read_data_from_ACC(PARA3_R_ADDR, buffer, 4);
				data3.uc[0] = buffer[0];
				data3.uc[1] = buffer[1];
				data3.uc[2] = buffer[2];
				data3.uc[3] = buffer[3];
    			tmp = data3.sint;
				A[r2][c2] -= tmp;
			}
			sem_post(&lock);
		}
		else{
			sem_wait(&lock);
			read_data_from_ACC(CORE1_R_ADDR, buffer, 4);
			data3.uc[0] = buffer[0];
			data3.uc[1] = buffer[1];
			data3.uc[2] = buffer[2];
			data3.uc[3] = buffer[3];
    		tmp = data3.sint;
			//printf("tmp2=%d \n", tmp);
			A[r1][c1] -= tmp;
			if((i+1) < end_idx3){
				read_data_from_ACC(PARA4_R_ADDR, buffer, 4);
				data3.uc[0] = buffer[0];
				data3.uc[1] = buffer[1];
				data3.uc[2] = buffer[2];
				data3.uc[3] = buffer[3];
    			tmp = data3.sint;
				A[r2][c2] -= tmp;
			}
		}
	}

	barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);
	i +=1;
	if(hart_id == 0){
		for (int j = 2; j < n; ++j) {
        	A[j][i] = A[j][i] << 6;
        	A[j][i] /= A[i][i];
				//printf("j=%d \n", j);
      	}
	}
	barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);
	sem_init(&lock, 0);
	int total_terms4 = 4;
	int num_terms_per_thread4 = (total_terms4 + nc - 1) / nc;
	int begin_idx4 = num_terms_per_thread4 * hart_id;
	int end_idx4;

	if (hart_id == 1) {
		end_idx4 = total_terms4;
	} else {
		end_idx4 = begin_idx4 + num_terms_per_thread4;
	}

	for (int i = begin_idx4; i < end_idx4; i=i+2) {
		int r1 = i / 2 + 2;
		int c1 = i % 2 + 2;
		int r2 = (i+1) / 2 + 2;
		int c2 = (i+1) % 2 + 2;
		printf("end_idx4=%d \n", end_idx4);
		//barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);
		if(hart_id == 0){
			data.sint = A[r1][1];
			buffer[0] = data.uc[0];
			buffer[1] = data.uc[1];
			buffer[2] = data.uc[2];
			buffer[3] = data.uc[3];
			write_data_to_ACC(CORE0_W1_ADDR, buffer, 4);
			//printf("A[j][i]=%d \n", A[j][i]);
			data2.sint = A[1][c1];
			buffer[0] = data2.uc[0];
			buffer[1] = data2.uc[1];
			buffer[2] = data2.uc[2];
			buffer[3] = data2.uc[3];
			write_data_to_ACC(CORE0_W2_ADDR, buffer, 4);
			//printf("A[i][k]=%d \n", A[i][k]);
			if((i+1) < end_idx4){
				data.sint = A[r2][1];
				buffer[0] = data.uc[0];
				buffer[1] = data.uc[1];
				buffer[2] = data.uc[2];
				buffer[3] = data.uc[3];
				write_data_to_ACC(PARA3_W1_ADDR, buffer, 4);
				//printf("A[j][i]=%d \n", A[j][i]);
				data2.sint = A[1][c2];
				buffer[0] = data2.uc[0];
				buffer[1] = data2.uc[1];
				buffer[2] = data2.uc[2];
				buffer[3] = data2.uc[3];
				write_data_to_ACC(PARA3_W2_ADDR, buffer, 4);
			}
			sem_post(&lock);
		}
		else{
			sem_wait(&lock);
			data.sint = A[r1][1];
			buffer[0] = data.uc[0];
			buffer[1] = data.uc[1];
			buffer[2] = data.uc[2];
			buffer[3] = data.uc[3];
			write_data_to_ACC(CORE1_W1_ADDR, buffer, 4);
			data2.sint = A[1][c1];
			buffer[0] = data2.uc[0];
			buffer[1] = data2.uc[1];
			buffer[2] = data2.uc[2];
			buffer[3] = data2.uc[3];
			write_data_to_ACC(CORE1_W2_ADDR, buffer, 4);
			//printf("A[i][k]=%d \n", A[i][k]);
			if((i+1) < end_idx4){
				data.sint = A[r2][1];
				buffer[0] = data.uc[0];
				buffer[1] = data.uc[1];
				buffer[2] = data.uc[2];
				buffer[3] = data.uc[3];
				write_data_to_ACC(PARA4_W1_ADDR, buffer, 4);
				//printf("A[j][i]=%d \n", A[j][i]);
				data2.sint = A[1][c2];
				buffer[0] = data2.uc[0];
				buffer[1] = data2.uc[1];
				buffer[2] = data2.uc[2];
				buffer[3] = data2.uc[3];
				write_data_to_ACC(PARA4_W2_ADDR, buffer, 4);
			}
		}
		barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);
		if(hart_id == 0){
			read_data_from_ACC(CORE0_R_ADDR, buffer, 4);
			data3.uc[0] = buffer[0];
			data3.uc[1] = buffer[1];
			data3.uc[2] = buffer[2];
			data3.uc[3] = buffer[3];
    		tmp = data3.sint;
			A[r1][c1] -= tmp;
			//printf("tmp=%d \n", tmp);
			if((i+1) < end_idx4){
				read_data_from_ACC(PARA3_R_ADDR, buffer, 4);
				data3.uc[0] = buffer[0];
				data3.uc[1] = buffer[1];
				data3.uc[2] = buffer[2];
				data3.uc[3] = buffer[3];
    			tmp = data3.sint;
				A[r2][c2] -= tmp;
				printf("r2=%dc2=%d \n", r2,c2);
				printf("tmp=%d \n", tmp);
			}
			sem_post(&lock);
		}
		else{
			sem_wait(&lock);
			read_data_from_ACC(CORE1_R_ADDR, buffer, 4);
			data3.uc[0] = buffer[0];
			data3.uc[1] = buffer[1];
			data3.uc[2] = buffer[2];
			data3.uc[3] = buffer[3];
    		tmp = data3.sint;
			//printf("tmp2=%d \n", tmp);
			A[r1][c1] -= tmp;
			if((i+1) < end_idx4){
				read_data_from_ACC(PARA4_R_ADDR, buffer, 4);
				data3.uc[0] = buffer[0];
				data3.uc[1] = buffer[1];
				data3.uc[2] = buffer[2];
				data3.uc[3] = buffer[3];
    			tmp = data3.sint;
				A[r2][c2] -= tmp;
			}
		}
		//barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);
	}
	barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);
	i +=1;
	if(hart_id == 0){
		for (int j = 3; j < n; ++j) {
        	A[j][i] = A[j][i] << 6;
        	A[j][i] /= A[i][i];
				//printf("j=%d \n", j);
      	}
	}
	barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);

	if(hart_id == 0){
		data.sint = A[3][2];
		buffer[0] = data.uc[0];
		buffer[1] = data.uc[1];
		buffer[2] = data.uc[2];
		buffer[3] = data.uc[3];
		write_data_to_ACC(CORE0_W1_ADDR, buffer, 4);
		//printf("A[j][i]=%d \n", A[j][i]);
		data2.sint = A[2][3];
		buffer[0] = data2.uc[0];
		buffer[1] = data2.uc[1];
		buffer[2] = data2.uc[2];
		buffer[3] = data2.uc[3];
		write_data_to_ACC(CORE0_W2_ADDR, buffer, 4);
		//printf("A[i][k]=%d \n", A[i][k]);
		read_data_from_ACC(CORE0_R_ADDR, buffer, 4);
		data3.uc[0] = buffer[0];
		data3.uc[1] = buffer[1];
		data3.uc[2] = buffer[2];
		data3.uc[3] = buffer[3];
    	tmp = data3.sint;
		A[3][3] -= tmp;		
	}
	barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);
	/*if (hart_id == 0) { 		
  		for (j = 0; j != n; ++j){
			result = x[j];
			r[j] = result / (1 << 10);
			sprintfloat(print, r[j]);
			printf("answer = %s\n", print);
			//printf("%d\n", y[j]);
            printf("%d\n", A_ori[0][j]);
			printf("%d\n", b_ori[j]); 
		  }   		
	}*/
	if (hart_id == 0) { 		
  		for (i = 0; i != n; ++i){
			for(j = 0; j != n; ++j){
				printf("%d\n", A[i][j]);
			}
		}  		
	}
	//barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);

	return 0;
}
