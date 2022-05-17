#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
#include <cmath>
#include <iomanip>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

struct gaussianFilter : public sc_module {
  tlm_utils::simple_target_socket<gaussianFilter> tsock;

  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  sc_fifo<int> o_red;
  sc_fifo<int> o_green;
  sc_fifo<int> o_blue;

  SC_HAS_PROCESS(gaussianFilter);

  gaussianFilter(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &gaussianFilter::blocking_transport);
    SC_THREAD(do_filter);
  }

  ~gaussianFilter() {
	}

  const int mask[MASK_X][MASK_Y] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
  int pix[3][258][3] ;
  int x = 0;
  int y = 0;
  unsigned int base_offset;

  void do_filter(){
    { wait(CLOCK_PERIOD, SC_NS); }
    while (true) {
      int red = 0;
      int green = 0;
      int blue = 0;

      pix[2][x][0] = i_r.read();
      pix[2][x][1] = i_g.read();
      pix[2][x][2] = i_b.read();
      wait(CLOCK_PERIOD, SC_NS);

      if(x >= 2 && y >= 2){
        for (unsigned int v = 0; v < MASK_Y; ++v) {
          for (unsigned int u = 0; u < MASK_X; ++u) {
            red += pix[v][u+x-2][0] * mask[v][u];
	          green += pix[v][u+x-2][1] * mask[v][u];
	          blue += pix[v][u+x-2][2] * mask[v][u];
            wait(CLOCK_PERIOD, SC_NS);
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
          wait(CLOCK_PERIOD, SC_NS);
        }
        x = 0;
        y = y + 1;
        wait(CLOCK_PERIOD, SC_NS);
      }
      else{
        x = x + 1;
        wait(CLOCK_PERIOD, SC_NS);
      }
    }
  }

  void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay){
    wait(delay);
    // unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    // auto len = payload.get_data_length();
    tlm::tlm_command cmd = payload.get_command();
    sc_dt::uint64 addr = payload.get_address();
    unsigned char *data_ptr = payload.get_data_ptr();

    addr -= base_offset;


    // cout << (int)data_ptr[0] << endl;
    // cout << (int)data_ptr[1] << endl;
    // cout << (int)data_ptr[2] << endl;
    word buffer;

    switch (cmd) {
      case tlm::TLM_READ_COMMAND:
        // cout << "READ" << endl;
        switch (addr) {
          case SOBEL_FILTER_RESULT_ADDR:
            buffer.uc[0] = o_red.read();
            buffer.uc[1] = o_green.read();
            buffer.uc[2] = o_blue.read();
            break;
          default:
            std::cerr << "READ Error! SobelFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
          }
        data_ptr[0] = buffer.uc[0];
        data_ptr[1] = buffer.uc[1];
        data_ptr[2] = buffer.uc[2];
        data_ptr[3] = buffer.uc[3];
        break;
      case tlm::TLM_WRITE_COMMAND:
        // cout << "WRITE" << endl;
        switch (addr) {
          case SOBEL_FILTER_R_ADDR:
            i_r.write(data_ptr[0]);
            i_g.write(data_ptr[1]);
            i_b.write(data_ptr[2]);
            break;
          default:
            std::cerr << "WRITE Error! SobelFilter::blocking_transport: address 0x"
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
};
#endif
