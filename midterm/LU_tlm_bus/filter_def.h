#ifndef FILTER_DEF_H_
#define FILTER_DEF_H_

// Sobel Filter inner transport addresses
// Used between blocking_transport() & do_filter()
const int SOBEL_FILTER_RN_ADDR = 0x00000000;
const int SOBEL_FILTER_RA_ADDR = 0x00000004;
const int SOBEL_FILTER_RB_ADDR = 0x00000008;
const int SOBEL_FILTER_RESULT_ADDR = 0x0000000c;
const int SOBEL_FILTER_CHECK_ADDR = 0x0000010;

union word {
  int sint;
  float fd;
  unsigned int uint;
  unsigned char uc[4];
};

#endif
