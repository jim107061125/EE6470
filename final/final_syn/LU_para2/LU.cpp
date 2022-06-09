#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

  #include "LU.h"

LU::LU( sc_module_name n ): sc_module( n )
{
#ifndef NATIVE_SYSTEMC
	HLS_FLATTEN_ARRAY(A);
  HLS_FLATTEN_ARRAY(x);
  HLS_FLATTEN_ARRAY(y);
  HLS_FLATTEN_ARRAY(b);
#endif
  //HLS_FLATTEN_ARRAY(conv);
	SC_THREAD( do_solve );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
        
#ifndef NATIVE_SYSTEMC
	i_n.clk_rst(i_clk, i_rst);
  i_a.clk_rst(i_clk, i_rst);
  i_b.clk_rst(i_clk, i_rst);
  o_x.clk_rst(i_clk, i_rst);
#endif
}

LU::~LU() {}

void LU::do_solve() {
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_n.reset();
    i_a.reset();
    i_b.reset();
		o_x.reset();
#endif
		wait();
	}
  sc_dt::sc_int<32> n;
#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("input1");
          //HLS_CONSTRAIN_LATENCY(0, 3, "lat01");
					n = i_n.get();
					wait();
				}
#else
				n = i_n.read();
#endif

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
#ifndef NATIVE_SYSTEMC
    {
      HLS_DEFINE_PROTOCOL("input2");
      //HLS_CONSTRAIN_LATENCY(0, 15, "lat02");
      A[i][j] = i_a.get();
      wait();
    }
#else
      A[i][j] = i_a.read();
#endif
    }
  }

  for (int i = 0; i < n; ++i) {
#ifndef NATIVE_SYSTEMC
  {
    HLS_DEFINE_PROTOCOL("input3");
    //HLS_CONSTRAIN_LATENCY(0, 5, "lat03");
    b[i] = i_b.get();
    wait();
  }
#else
    b[i] = i_b.read();
#endif
  }

  //lu decomposition
  for (int i = 0; i < n; ++i) {
    for (int j = i+1; j < n; ++j) {
      HLS_CONSTRAIN_LATENCY(0, 4, "lat04");
      //HLS_UNROLL_LOOP(ON, "SHIFT1");
      A[j][i] = A[j][i] << 6;
      A[j][i] /= A[i][i];
      //A[j][i] = tmp;
    }

    for (int j = i+1; j < n; ++j) {
      for (int k = i+1; k < n; ++k) {
        HLS_CONSTRAIN_LATENCY(0, 2, "lat05");
        //HLS_UNROLL_LOOP(ON, "SHIFT2");
        A[j][k] -= (A[j][i] * A[i][k]) >> 6;
      }
    }
  }
  
  //forward substitution
  for (int i = 0; i < n; ++i) {
    //HLS_CONSTRAIN_LATENCY(0, 5, "lat06");
    //HLS_UNROLL_LOOP(ON, "SHIFT4");
    y[i] = b[i];
  }

  for (int i = 0; i < n; ++i) {
    for (int j = i+1; j < n; ++j) {
      HLS_CONSTRAIN_LATENCY(0, 2, "lat07");
      //HLS_UNROLL_LOOP(ON, "SHIFT3");
      y[j] -= (A[j][i] * y[i]) >> 6;
    }
  }

  //backward substitution
  for (int i = 0; i < n; ++i) {
    //HLS_CONSTRAIN_LATENCY(0, 5, "lat08");
    //HLS_UNROLL_LOOP(ON, "SHIFT4");
    x[i] = y[i];
  }

  for (int i = n-1; i >= 0; i--) {
    HLS_CONSTRAIN_LATENCY(0, 4, "lat09");
    x[i] = x[i] << 6;
    x[i] /= A[i][i];
    //x[i] = tmp;
    for (int j = i-1; j >= 0; j--) {
      HLS_CONSTRAIN_LATENCY(0, 2, "lat010");
      //HLS_UNROLL_LOOP(ON, "SHIFT4");
      x[j] -= (A[j][i] * x[i]) >> 6;
    }
  } 

  //write back
  for (int i = 0; i < n; ++i) {
#ifndef NATIVE_SYSTEMC
		  {
			  HLS_DEFINE_PROTOCOL("output");
        //HLS_CONSTRAIN_LATENCY(0, 3, "lat11");
			  o_x.put(x[i]);
			  wait();
		  }
#else
		  o_x.write(x[i]);
#endif
  }  
}
