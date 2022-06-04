#ifndef FILTER_DEF2_H_
#define FILTER_DEF2_H_

#define CLOCK_PERIOD 10


#define MAX_IMAGE_BUFFER_LENTH 1024
#define THRESHOLD 90

// sobel mask parameters
const int MASK_N = 2;
const int MASK_X = 3;
const int MASK_Y = 3;
const int DMA_TRANS = 64;

// Sobel Filter inner transport addresses
// Used between blocking_transport() & do_filter()
//const int SOBEL_FILTER_N_ADDR = 0x00000000;
//const int SOBEL_FILTER_A_ADDR = 0x00000004;
//const int SOBEL_FILTER_B_ADDR = 0x00000008;
//const int SOBEL_FILTER_RESULT_ADDR = 0x0000000C;
//const int SOBEL_FILTER_DONE_ADDR = 0x00000010;
const int CORE1_W_ADDR = 0x00000000;
const int CORE1_R_ADDR = 0x00000010;

const int SOBEL_FILTER_RS_R_ADDR   = 0x00000000;
const int SOBEL_FILTER_RS_W_WIDTH  = 0x00000004;
const int SOBEL_FILTER_RS_W_HEIGHT = 0x00000008;
const int SOBEL_FILTER_RS_W_DATA   = 0x0000000C;
const int SOBEL_FILTER_RS_RESULT_ADDR = 0x00800000;


/*union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};*/

#endif
