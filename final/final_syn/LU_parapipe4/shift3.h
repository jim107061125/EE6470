#ifndef shift3_H_
#define shift3_H_
#define SC_INCLUDE_FX
#include <systemc>
using namespace sc_core;
using namespace std;
using namespace sc_dt;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "filter_def.h"

class shift3: public sc_module
{
public:
	sc_in_clk i_clk;
	sc_in < bool >  i_rst;
#ifndef NATIVE_SYSTEMC
  cynw_p2p< sc_int<32> >::in i_shiftin3;
  cynw_p2p< sc_int<32> >::out o_shiftout3;
#else
  sc_fifo_in< sc_int<32> > i_shiftin3;
	sc_fifo_out< sc_int<32> > o_shiftout3;
#endif

	SC_HAS_PROCESS( shift3 );
	shift3( sc_module_name n );
	~shift3();
private:
	void do_solve();
  //int val[MASK_N];
  //sc_dt::sc_uint<8> conv[3][3][3];
  //sc_dt::sc_int<24> A[10][10];
  //sc_dt::sc_int<24> b[10];
  //sc_dt::sc_int<24> x[10];
  //sc_dt::sc_int<24> y[10];
  //sc_dt::sc_int<48> tmp;
};
#endif
