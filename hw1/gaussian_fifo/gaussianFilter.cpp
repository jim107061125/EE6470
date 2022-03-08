#include <cmath>

#include "gaussianFilter.h"

gaussianFilter::gaussianFilter(sc_module_name n) : sc_module(n) {
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);
}

// sobel mask
const int mask[MASK_X][MASK_Y] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};

void gaussianFilter::do_filter() {
  while (true) {
    
    int red = 0;
    int green = 0;
    int blue = 0;
    
    for (unsigned int v = 0; v < MASK_Y; ++v) {
      for (unsigned int u = 0; u < MASK_X; ++u) {
        red += i_r.read() * mask[u][v];
	green += i_g.read() * mask[u][v];
	blue += i_b.read() * mask[u][v];
      }
    }
    if((red/16) < 0){
      red = red * (-1) / 16;
    } 
    else{
      red = red / 16;
    }

    if(red < 0){
      red = 0;
    }
    else if(red > 255){
      red = 255;
    }
    else{
      red = red * 1;
    }

    if((green/16) < 0){
      green = green * (-1) / 16;
    }
    else{
      green = green / 16;
    }

    if(green < 0){
      green = 0;
    }
    else if(green > 255){
      green = 255;
    }
    else{
      green = green * 1;
    }
    
    if((blue/16) < 0){
      blue = blue * (-1) / 16;
    }
    else{
      blue = blue / 16;
    }

    if(blue < 0){
      blue = 0;
    }
    else if(blue > 255){
      blue = 255;
    }
    else{
      blue = blue * 1;
    }


    o_red.write(red);
    o_green.write(green);
    o_blue.write(blue);
    wait(10); //emulate module delay
  }
}
