#ifndef unit1_H_
#define unit4_H_
#define SC_INCLUDE_FX
#include <systemc>
using namespace sc_core;
using namespace std;
using namespace sc_dt;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "filter_def.h"

class unit4: public sc_module
{
public:
	sc_in_clk i_clk;
	sc_in < bool >  i_rst;
#ifndef NATIVE_SYSTEMC
  cynw_p2p< sc_int<32> >::in i_u0;
  cynw_p2p< sc_int<32> >::in i_u4;
  cynw_p2p< sc_int<32> >::out o_result;
#else
  sc_fifo_in< sc_int<32> > i_u0;
  sc_fifo_in< sc_int<32> > i_u4;
	sc_fifo_out< sc_int<32> > o_result;
#endif

	SC_HAS_PROCESS( unit4 );
	unit4( sc_module_name n );
	~unit4();
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
