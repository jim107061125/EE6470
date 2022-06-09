#ifndef SYSTEM_H_
#define SYSTEM_H_
#include <systemc>
using namespace sc_core;

#include "Testbench.h"
#ifndef NATIVE_SYSTEMC
#include "unit1_wrap.h"
#else
#include "unit1.h"
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
#else
	unit1 unit_1;
#endif
	sc_clock clk;
	sc_signal<bool> rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_int<32> > in1;
  cynw_p2p< sc_dt::sc_int<32> > in2;
  cynw_p2p< sc_dt::sc_int<32> > result1;
#else
	sc_fifo< sc_dt::sc_int<32> > in1;
  sc_fifo< sc_dt::sc_int<32> > in2;
  sc_fifo< sc_dt::sc_int<32> > result1;
#endif

	//std::string _output_bmp;
};
#endif
