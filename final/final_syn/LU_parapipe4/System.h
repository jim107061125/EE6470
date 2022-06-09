#ifndef SYSTEM_H_
#define SYSTEM_H_
#include <systemc>
using namespace sc_core;

#include "Testbench.h"
#ifndef NATIVE_SYSTEMC
#include "unit1_wrap.h"
#include "unit2_wrap.h"
#include "unit3_wrap.h"
#include "unit4_wrap.h"
#include "shift1_wrap.h"
#include "shift2_wrap.h"
#include "shift3_wrap.h"
#include "shift4_wrap.h"
#else
#include "unit1.h"
#include "unit2.h"
#include "unit3.h"
#include "unit4.h"
#include "shift1.h"
#include "shift2.h"
#include "shift3.h"
#include "shift4.h"
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
  unit3_wrapper unit_3;
  unit4_wrapper unit_4;
  shift1_wrapper shift_1;
  shift2_wrapper shift_2;
  shift3_wrapper shift_3;
  shift4_wrapper shift_4;
#else
	unit1 unit_1;
  unit2 unit_2;
  unit3 unit_3;
  unit4 unit_4;
  shift1 shift_1;
  shift2 shift_2;
  shift3 shift_3;
  shift4 shift_4;
#endif
	sc_clock clk;
	sc_signal<bool> rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_int<32> > in1;
  cynw_p2p< sc_dt::sc_int<32> > in2;
  cynw_p2p< sc_dt::sc_int<32> > in3;
	cynw_p2p< sc_dt::sc_int<32> > in4;
  cynw_p2p< sc_dt::sc_int<32> > in5;
  cynw_p2p< sc_dt::sc_int<32> > in6;
  cynw_p2p< sc_dt::sc_int<32> > in7;
	cynw_p2p< sc_dt::sc_int<32> > in8;
  cynw_p2p< sc_dt::sc_int<32> > result1;
  cynw_p2p< sc_dt::sc_int<32> > result2;
  cynw_p2p< sc_dt::sc_int<32> > result3;
  cynw_p2p< sc_dt::sc_int<32> > result4;
  cynw_p2p< sc_dt::sc_int<32> > shiftin1;
  cynw_p2p< sc_dt::sc_int<32> > shiftin2;
  cynw_p2p< sc_dt::sc_int<32> > shiftin3;
  cynw_p2p< sc_dt::sc_int<32> > shiftin4;
#else
	sc_fifo< sc_dt::sc_int<32> > in1;
  sc_fifo< sc_dt::sc_int<32> > in2;
  sc_fifo< sc_dt::sc_int<32> > in3;
	sc_fifo< sc_dt::sc_int<32> > in4;
  sc_fifo< sc_dt::sc_int<32> > in5;
  sc_fifo< sc_dt::sc_int<32> > in6;
  sc_fifo< sc_dt::sc_int<32> > in7;
	sc_fifo< sc_dt::sc_int<32> > in8;
  sc_fifo< sc_dt::sc_int<32> > result1;
  sc_fifo< sc_dt::sc_int<32> > result2;
  sc_fifo< sc_dt::sc_int<32> > result3;
  sc_fifo< sc_dt::sc_int<32> > result4;
  sc_fifo< sc_dt::sc_int<32> > shiftin1;
  sc_fifo< sc_dt::sc_int<32> > shiftin2;
  sc_fifo< sc_dt::sc_int<32> > shiftin3;
  sc_fifo< sc_dt::sc_int<32> > shiftin4;
#endif

	//std::string _output_bmp;
};
#endif
