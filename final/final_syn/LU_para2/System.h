#ifndef SYSTEM_H_
#define SYSTEM_H_
#include <systemc>
using namespace sc_core;

#include "Testbench.h"
#ifndef NATIVE_SYSTEMC
#include "unit1_wrap.h"
#include "unit2_wrap.h"
#else
#include "unit1.h"
#include "unit2.h"
#endif

class System: public sc_module
{
public:
	SC_HAS_PROCESS( System );
	System( sc_module_name name );
	~System();
private:
  Testbench tb;
#ifndef NATIVE_SYSTEMC
	unit1_wrapper unit_1;
  unit2_wrapper unit_2;
#else
	unit1 unit_1;
  unit2 unit_2;
#endif
	sc_clock clk;
	sc_signal<bool> rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_int<32> > in1;
  cynw_p2p< sc_dt::sc_int<32> > in2;
  cynw_p2p< sc_dt::sc_int<32> > in3;
	cynw_p2p< sc_dt::sc_int<32> > in4;
  cynw_p2p< sc_dt::sc_int<32> > result1;
  cynw_p2p< sc_dt::sc_int<32> > result2;
#else
	sc_fifo< sc_dt::sc_int<32> > in1;
  sc_fifo< sc_dt::sc_int<32> > in2;
  sc_fifo< sc_dt::sc_int<32> > in3;
	sc_fifo< sc_dt::sc_int<32> > in4;
  sc_fifo< sc_dt::sc_int<32> > result1;
  sc_fifo< sc_dt::sc_int<32> > result2;
#endif

	//std::string _output_bmp;
};
#endif
