#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

  #include "shift3.h"

shift3::shift3( sc_module_name n ): sc_module( n )
{
/*#ifndef NATIVE_SYSTEMC
	HLS_FLATTEN_ARRAY(A);
  HLS_FLATTEN_ARRAY(x);
  HLS_FLATTEN_ARRAY(y);
  HLS_FLATTEN_ARRAY(b);
#endif*/
  //HLS_FLATTEN_ARRAY(conv);
	SC_THREAD( do_solve );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
        
#ifndef NATIVE_SYSTEMC
	i_shiftin3.clk_rst(i_clk, i_rst);
  o_shiftout3.clk_rst(i_clk, i_rst);
#endif
}

shift3::~shift3() {}

void shift3::do_solve() {
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_shiftin3.reset();
		o_shiftout3.reset();
#endif
		wait();
	}
  sc_dt::sc_int<32> shift3;
	while (true) {
#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("input1");
          //HLS_CONSTRAIN_LATENCY(0, 3, "lat01");
					shift3 = i_shiftin3.get();
					wait();
				}
#else
				shift3 = i_shiftin3.read();
#endif

		int result = 0;
    HLS_CONSTRAIN_LATENCY(0, 5, "lat01");
    result = shift3 >> 6;


  //write back
#ifndef NATIVE_SYSTEMC
		  {
			  HLS_DEFINE_PROTOCOL("output");
        //HLS_CONSTRAIN_LATENCY(0, 3, "lat11");
			  o_shiftout3.put(result);
			  wait();
		  }
#else
		  o_shiftout3.write(result);
#endif
  }

}

