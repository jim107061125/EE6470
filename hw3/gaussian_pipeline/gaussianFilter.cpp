#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

#include "gaussianFilter.h"

gaussianFilter::gaussianFilter( sc_module_name n ): sc_module( n )
{
//#ifndef NATIVE_SYSTEMC
//	HLS_FLATTEN_ARRAY(val);
//#endif
  HLS_FLATTEN_ARRAY(conv);
	SC_THREAD( do_filter );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
        
#ifndef NATIVE_SYSTEMC
	i_rgb.clk_rst(i_clk, i_rst);
  o_result.clk_rst(i_clk, i_rst);
#endif
}

gaussianFilter::~gaussianFilter() {}

// gaussian mask
const int mask[MASK_X][MASK_Y] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
int pix[3][258][3];
int x = 0;
int y = 0;

void gaussianFilter::do_filter() {
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_rgb.reset();
		o_result.reset();
#endif
		wait();
	}
  for (unsigned int a = 0; a<3; ++a) {
    for (unsigned int b = 0; b<258; ++b) {
      pix[a][b][0] = 0;
      pix[a][b][1] = 0;
      pix[a][b][2] = 0;
    }			  
  }
  for (unsigned int a = 0; a<3; ++a) {
    for (unsigned int b = 0; b<3; ++b) {
      conv[a][b][0] = 0;
      conv[a][b][1] = 0;
      conv[a][b][2] = 0;
    }			  
  }
	while (true) {
		/*for (unsigned int i = 0; i<MASK_N; ++i) {
			HLS_CONSTRAIN_LATENCY(0, 1, "lat00");
			val[i] = 0;
		}*/
    int red = 0;
    int green = 0;
    int blue = 0;
    sc_dt::sc_uint<24> rgb;
    sc_dt::sc_uint<32> total;
#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("input");
					rgb = i_rgb.get();
					wait();
				}
#else
				rgb = i_rgb.read();
#endif

    pix[2][x][0] = rgb.range(7,0);
    pix[2][x][1] = rgb.range(15,8);
    pix[2][x][2] = rgb.range(23,16);
    
    
    if(x >= 2 && y >= 2){
      for (unsigned int b = 0; b < 2; ++b) {
        HLS_UNROLL_LOOP(ON, "SHIFT");
	      conv[0][b][0] = conv[0][b+1][0];
	      conv[0][b][1] = conv[0][b+1][1];
	      conv[0][b][2] = conv[0][b+1][2];
	      conv[1][b][0] = conv[1][b+1][0];
        conv[1][b][1] = conv[1][b+1][1];
        conv[1][b][2] = conv[1][b+1][2];
        conv[2][b][0] = conv[2][b+1][0];
        conv[2][b][1] = conv[2][b+1][1];
        conv[2][b][2] = conv[2][b+1][2];
      }
      conv[0][2][0] = pix[0][x][0];
      conv[0][2][1] = pix[0][x][1];
      conv[0][2][2] = pix[0][x][2];
      conv[1][2][0] = pix[1][x][0];
      conv[1][2][1] = pix[1][x][1];
      conv[1][2][2] = pix[1][x][2];
      conv[2][2][0] = pix[2][x][0];
      conv[2][2][1] = pix[2][x][1];
      conv[2][2][2] = pix[2][x][2];
		  for (unsigned int v = 0; v<MASK_Y; ++v) {
        //HLS_UNROLL_LOOP(ON, "SHIFT");
			  for (unsigned int u = 0; u<MASK_X; ++u) {
				  //unsigned char grey = (rgb.range(7,0) + rgb.range(15,8) + rgb.range(23, 16))/3;
          HLS_UNROLL_LOOP(ON, "SHIFT");
          HLS_PIPELINE_LOOP(HARD_STALL, 1, "hard stall");
				  HLS_CONSTRAIN_LATENCY(0, 1, "lat01");
				  red += conv[v][u][0] * mask[v][u];
          green += conv[v][u][1] * mask[v][u];
          blue += conv[v][u][2] * mask[v][u];				
			  }
		  }
      red = red >> 4;
      green = green >> 4;
      blue = blue >> 4;
      
      //int total = 0;
      total.range(7,0) = red;
      total.range(15,8) = green;
      total.range(23,16) = blue;

#ifndef NATIVE_SYSTEMC
		  {
			  HLS_DEFINE_PROTOCOL("output");
			  o_result.put(total);
			  wait();
		  }
#else
		  o_result.write(total);
#endif
    }
    else{
      for (unsigned int b = 0; b < 2; ++b) {
        HLS_UNROLL_LOOP(ON, "SHIFT");
	      conv[0][b+1][0] = pix[0][b][0];
	      conv[0][b+1][1] = pix[0][b][1];
	      conv[0][b+1][2] = pix[0][b][2];
	      conv[1][b+1][0] = pix[1][b][0];
        conv[1][b+1][1] = pix[1][b][1];
        conv[1][b+1][2] = pix[1][b][2];
        conv[2][b+1][0] = pix[2][b][0];
        conv[2][b+1][1] = pix[2][b][1];
        conv[2][b+1][2] = pix[2][b][2];
      }
    }
    
    if(x == 257){
      for (unsigned int b = 0; b < 258; ++b) {        
	      pix[0][b][0] = pix[1][b][0];
	      pix[0][b][1] = pix[1][b][1];
	      pix[0][b][2] = pix[1][b][2];
	      pix[1][b][0] = pix[2][b][0];
        pix[1][b][1] = pix[2][b][1];
        pix[1][b][2] = pix[2][b][2];
      }
      x = 0;
      y = y + 1;
    }
    else{
      x = x + 1;
    }
	}
}
