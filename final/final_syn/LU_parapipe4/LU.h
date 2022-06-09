#ifndef LU_H_
#define LU_H_
#define SC_INCLUDE_FX
#include <systemc>
using namespace sc_core;
using namespace std;
using namespace sc_dt;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "filter_def.h"

class LU: public sc_module
{
public:
	sc_in_clk i_clk;
	sc_in < bool >  i_rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_int<5> >::in i_n;
  cynw_p2p< sc_int<24> >::in i_a;
  cynw_p2p< sc_int<24> >::in i_b;
	cynw_p2p< sc_int<24> >::out o_x;
#else
	sc_fifo_in< sc_int<5> > i_n;
  sc_fifo_in< sc_int<24> > i_a;
  sc_fifo_in< sc_int<24> > i_b;
	sc_fifo_out< sc_int<24> > o_x;
#endif

	SC_HAS_PROCESS( LU );
	LU( sc_module_name n );
	~LU();
private:
	void do_solve();
  //int val[MASK_N];
  //sc_dt::sc_uint<8> conv[3][3][3];
  sc_dt::sc_int<24> A[10][10];
  sc_dt::sc_int<24> b[10];
  sc_dt::sc_int<24> x[10];
  sc_dt::sc_int<24> y[10];
  //sc_dt::sc_int<48> tmp;
};
#endif
