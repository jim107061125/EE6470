#include "System.h"
System::System( sc_module_name name ): sc_module( name ), 
	tb("tb"), unit_1("unit_1"), unit_2("unit_2"), unit_3("unit_3"), unit_4("unit_4"), shift_1("shift_1"), shift_2("shift_2"), shift_3("shift_3"), shift_4("shift_4"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst")
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
  shift_1.i_clk(clk);
	shift_1.i_rst(rst);
  shift_2.i_clk(clk);
	shift_2.i_rst(rst);
  shift_3.i_clk(clk);
	shift_3.i_rst(rst);
  shift_4.i_clk(clk);
	shift_4.i_rst(rst);
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
  unit_1.o_result(shiftin1);
  unit_2.i_u0(in3);
  unit_2.i_u2(in4);
  unit_2.o_result(shiftin2);
  unit_3.i_u0(in5);
  unit_3.i_u3(in6);
  unit_3.o_result(shiftin3);
  unit_4.i_u0(in7);
  unit_4.i_u4(in8);
  unit_4.o_result(shiftin4);
  shift_1.i_shiftin1(shiftin1);
  shift_1.o_shiftout1(result1);
  shift_2.i_shiftin2(shiftin2);
  shift_2.o_shiftout2(result2);
  shift_3.i_shiftin3(shiftin3);
  shift_3.o_shiftout3(result3);
  shift_4.i_shiftin4(shiftin4);
  shift_4.o_shiftout4(result4);

  //tb.read_bmp(input_bmp);
}

System::~System() {
  //tb.write_bmp(_output_bmp);
}
