#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
using namespace sc_core;

#include "filter_def.h"

class gaussianFilter : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  sc_fifo_in<unsigned char> i_r;
  sc_fifo_in<unsigned char> i_g;
  sc_fifo_in<unsigned char> i_b;
  sc_fifo_in<int> i_x;
  sc_fifo_in<int> i_y;
  sc_fifo_out<int> o_red;
  sc_fifo_out<int> o_green;
  sc_fifo_out<int> o_blue;

  SC_HAS_PROCESS(gaussianFilter);
  gaussianFilter(sc_module_name n);
  ~gaussianFilter() = default;

private:
  void do_filter();
};
#endif
