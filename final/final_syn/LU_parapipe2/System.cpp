#include "System.h"
System::System( sc_module_name name ): sc_module( name ), 
	tb("tb"), unit_1("unit_1"), unit_2("unit_2"), shift_1("shift_1"), shift_2("shift_2"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst")
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	unit_1.i_clk(clk);
	unit_1.i_rst(rst);
  unit_2.i_clk(clk);
	unit_2.i_rst(rst);
  shift_1.i_clk(clk);
	shift_1.i_rst(rst);
  shift_2.i_clk(clk);
	shift_2.i_rst(rst);
	tb.o_u1(in1);
  tb.o_u2(in2);
  tb.o_u3(in3);
  tb.o_u4(in4);
	tb.i_result1(result1);
  tb.i_result2(result2);
	unit_1.i_u0(in1);
  unit_1.i_u1(in2);
  unit_1.o_result(shiftin1);
  unit_2.i_u0(in3);
  unit_2.i_u2(in4);
  unit_2.o_result(shiftin2);
  shift_1.i_shiftin1(shiftin1);
  shift_1.o_shiftout1(result1);
  shift_2.i_shiftin2(shiftin2);
  shift_2.o_shiftout2(result2);

  //tb.read_bmp(input_bmp);
}

System::~System() {
  //tb.write_bmp(_output_bmp);
}
