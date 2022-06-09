#include "System.h"
System::System( sc_module_name name ): sc_module( name ), 
	tb("tb"), unit_1("unit_1"), unit_2("unit_2"), unit_3("unit_3"), unit_4("unit_4"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst")
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	unit_1.i_clk(clk);
	unit_1.i_rst(rst);
  unit_2.i_clk(clk);
	unit_2.i_rst(rst);
  unit_3.i_clk(clk);
	unit_3.i_rst(rst);
  unit_4.i_clk(clk);
	unit_4.i_rst(rst);
	tb.o_u1(in1);
  tb.o_u2(in2);
  tb.o_u3(in3);
  tb.o_u4(in4);
  tb.o_u5(in5);
  tb.o_u6(in6);
  tb.o_u7(in7);
  tb.o_u8(in8);
	tb.i_result1(result1);
  tb.i_result2(result2);
  tb.i_result3(result3);
  tb.i_result4(result4);
	unit_1.i_u0(in1);
  unit_1.i_u1(in2);
  unit_1.o_result(result1);
  unit_2.i_u0(in3);
  unit_2.i_u2(in4);
  unit_2.o_result(result2);
  unit_3.i_u0(in5);
  unit_3.i_u3(in6);
  unit_3.o_result(result3);
  unit_4.i_u0(in7);
  unit_4.i_u4(in8);
  unit_4.o_result(result4);

  //tb.read_bmp(input_bmp);
}

System::~System() {
  //tb.write_bmp(_output_bmp);
}
