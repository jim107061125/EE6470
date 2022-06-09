#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

  #include "unit2.h"

unit2::unit2( sc_module_name n ): sc_module( n )
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
	i_u0.clk_rst(i_clk, i_rst);
  i_u2.clk_rst(i_clk, i_rst);
  o_result.clk_rst(i_clk, i_rst);
#endif
}

unit2::~unit2() {}

void unit2::do_solve() {
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_u0.reset();
    i_u2.reset();
		o_result.reset();
#endif
		wait();
	}
  sc_dt::sc_int<32> u0;
  sc_dt::sc_int<32> u2;
	while (true) {
#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("input1");
          //HLS_CONSTRAIN_LATENCY(0, 3, "lat01");
					u0 = i_u0.get();
					wait();
				}
#else
				u0 = i_u0.read();
#endif

#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("input2");
          //HLS_CONSTRAIN_LATENCY(0, 3, "lat01");
					u2 = i_u2.get();
					wait();
				}
#else
				u2 = i_u2.read();
#endif
		int result = 0;
    HLS_CONSTRAIN_LATENCY(0, 7, "lat01");
    result = u0 * u2;


  //write back
#ifndef NATIVE_SYSTEMC
		  {
			  HLS_DEFINE_PROTOCOL("output");
        //HLS_CONSTRAIN_LATENCY(0, 3, "lat1");
			  o_result.put(result);
			  wait();
		  }
#else
		  o_result.write(result);
#endif
  }

}

