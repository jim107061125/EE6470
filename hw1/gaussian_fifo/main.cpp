#include <iostream>
#include <string>
using namespace std;

// Wall Clock Time Measurement
#include <sys/time.h>

#include "gaussianFilter.h"
#include "Testbench.h"

// TIMEVAL STRUCT IS Defined ctime
// use start_time and end_time variables to capture
// start of simulation and end of simulation
struct timeval start_time, end_time;

// int main(int argc, char *argv[])
int sc_main(int argc, char **argv) {
  if ((argc < 3) || (argc > 4)) {
    cout << "No arguments for the executable : " << argv[0] << endl;
    cout << "Usage : >" << argv[0] << " in_image_file_name out_image_file_name"
         << endl;
    return 0;
  }

  //Create modules and signals
  Testbench tb("tb");
  gaussianFilter gaussian_filter("gaussian_filter");
  sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
  sc_signal<bool> rst("rst");

  //Create FIFO channels
  sc_fifo<unsigned char> r;
  sc_fifo<unsigned char> g;
  sc_fifo<unsigned char> b;
  sc_fifo<int> red;
  sc_fifo<int> green;
  sc_fifo<int> blue;

  //Connect FIFO channels with modules
  tb.i_clk(clk);
  tb.o_rst(rst);
  gaussian_filter.i_clk(clk);
  gaussian_filter.i_rst(rst);
  tb.o_r(r);
  tb.o_g(g);
  tb.o_b(b);
  tb.i_red(red);
  tb.i_green(green);
  tb.i_blue(blue);
  gaussian_filter.i_r(r);
  gaussian_filter.i_g(g);
  gaussian_filter.i_b(b);
  gaussian_filter.o_red(red);
  gaussian_filter.o_green(green);
  gaussian_filter.o_blue(blue);

  tb.read_bmp(argv[1]);
  sc_start();
  std::cout << "Simulated time == " << sc_core::sc_time_stamp() << std::endl;
  tb.write_bmp(argv[2]);

  return 0;
}
