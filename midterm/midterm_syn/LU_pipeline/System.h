#ifndef SYSTEM_H_
#define SYSTEM_H_
#include <systemc>
using namespace sc_core;

#include "Testbench.h"
#ifndef NATIVE_SYSTEMC
#include "LU_wrap.h"
#else
#include "LU.h"
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
	LU_wrapper L_U;
#else
	LU L_U;
#endif
	sc_clock clk;
	sc_signal<bool> rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_int<5> > n;
  cynw_p2p< sc_dt::sc_int<24> > a;
  cynw_p2p< sc_dt::sc_int<24> > b;
	cynw_p2p< sc_dt::sc_int<24> > x;
#else
	sc_fifo< sc_dt::sc_int<5> > n;
  sc_fifo< sc_dt::sc_int<24> > a;
  sc_fifo< sc_dt::sc_int<24> > b;
	sc_fifo< sc_dt::sc_int<24> > x;
#endif

	//std::string _output_bmp;
};
#endif
