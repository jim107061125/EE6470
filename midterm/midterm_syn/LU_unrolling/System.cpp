#include "System.h"
System::System( sc_module_name name ): sc_module( name ), 
	tb("tb"), L_U("L_U"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst")
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	L_U.i_clk(clk);
	L_U.i_rst(rst);
	tb.o_n(n);
  tb.o_a(a);
  tb.o_b(b);
	tb.i_x(x);
	L_U.i_n(n);
  L_U.i_a(a);
  L_U.i_b(b);
	L_U.o_x(x);

  //tb.read_bmp(input_bmp);
}

System::~System() {
  //tb.write_bmp(_output_bmp);
}
