#include <cmath>
#include <iomanip>

#include "gaussianFilter.h"

gaussianFilter::gaussianFilter(sc_module_name n)
    : sc_module(n), t_skt("t_skt"), base_offset(0) {
  SC_THREAD(do_filter);

  t_skt.register_b_transport(this, &gaussianFilter::blocking_transport);
}

//gaussianFilter::~gaussianFilter() = default;

// sobel mask
const int mask[MASK_X][MASK_Y] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
int pix[3][258][3] ;
int x = 0;
int y = 0;

void gaussianFilter::do_filter() {
  while (true) {
    
    int red = 0;
    int green = 0;
    int blue = 0;

    pix[2][x][0] = i_r.read();
    pix[2][x][1] = i_g.read();
    pix[2][x][2] = i_b.read();
				        
    if(x >= 2 && y >= 2){
      for (unsigned int v = 0; v < MASK_Y; ++v) {
        for (unsigned int u = 0; u < MASK_X; ++u) {
          red += pix[v][u+x-2][0] * mask[v][u];
	        green += pix[v][u+x-2][1] * mask[v][u];
	        blue += pix[v][u+x-2][2] * mask[v][u];
        }
      }
      if((red/16) < 0)
        red = red * (-1) / 16;
      else
        red = red / 16;

      if(red < 0)
        red = 0;
      else if(red > 255)
        red = 255;
      else
        red = red * 1;

      if((green/16) < 0)
        green = green * (-1) / 16;
      else
        green = green / 16;

      if(green < 0)
        green = 0;    
      else if(green > 255)
        green = 255;    
      else
        green = green * 1;
    
    
      if((blue/16) < 0)
        blue = blue * (-1) / 16;
      else
        blue = blue / 16;

      if(blue < 0)
        blue = 0;
      else if(blue > 255)
        blue = 255;
      else
        blue = blue * 1;

      o_red.write(red);
      o_green.write(green);
      o_blue.write(blue);
      //wait(1); //emulate module delay
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
    
    //else
      //wait(1);  
  }
}

void gaussianFilter::blocking_transport(tlm::tlm_generic_payload &payload,
                                     sc_core::sc_time &delay) {
  sc_dt::uint64 addr = payload.get_address();
  addr -= base_offset;
  unsigned char *mask_ptr = payload.get_byte_enable_ptr();
  unsigned char *data_ptr = payload.get_data_ptr();
  word buffer;
  switch (payload.get_command()) {
  case tlm::TLM_READ_COMMAND:
    switch (addr) {
    case SOBEL_FILTER_RESULT_ADDR:
      buffer.uc[0] = o_red.read();
      buffer.uc[1] = o_green.read();
      buffer.uc[2] = o_blue.read();
      break;
    case SOBEL_FILTER_CHECK_ADDR:  
      buffer.uc[0] = o_red.num_available();
      buffer.uc[1] = o_green.num_available();
      buffer.uc[2] = o_blue.num_available();
    break;
    default:
      std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
      buffer.uc[0] = 0;
      buffer.uc[1] = 0;
      buffer.uc[2] = 0;
      buffer.uc[3] = 0;
    }
    data_ptr[0] = buffer.uc[0];
    data_ptr[1] = buffer.uc[1];
    data_ptr[2] = buffer.uc[2];
    data_ptr[3] = buffer.uc[3];
    break;
  case tlm::TLM_WRITE_COMMAND:
    switch (addr) {
    case SOBEL_FILTER_R_ADDR:
      if (mask_ptr[0] == 0xff) {
        i_r.write(data_ptr[0]);
      }
      if (mask_ptr[1] == 0xff) {
        i_g.write(data_ptr[1]);
      }
      if (mask_ptr[2] == 0xff) {
        i_b.write(data_ptr[2]);
      }
      break;
    default:
      std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
    }
    break;
  case tlm::TLM_IGNORE_COMMAND:
    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    return;
  default:
    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    return;
  }
  payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
}
