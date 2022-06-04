//#ifndef SOBEL_FILTER_H_
//#define SOBEL_FILTER_H_
#include <systemc>
#include <cmath>
#include <iomanip>
#include <stdio.h>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

struct para_unit3 : public sc_module {
  tlm_utils::simple_target_socket<para_unit3> tsock;

  sc_fifo<int> o_u3;
  sc_fifo<int> i_u0;
  sc_fifo<int> i_u3;
  //sc_fifo<int> i_lo;
  //sc_fifo_in<int> i_up;

  SC_HAS_PROCESS(para_unit3);

  para_unit3(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &para_unit3::blocking_transport);
    SC_THREAD(do_compute);
  }

  ~para_unit3() {
	}

  int u0, u3;
  //int lo;
  int result;
  
  //int done;
  unsigned int base_offset;

  void do_compute(){
    while(true){
        { wait(CLOCK_PERIOD, SC_NS); }

        //lo = i_lo.read();
        u0 = i_u0.read();
        u3 = i_u3.read();
        wait(CLOCK_PERIOD, SC_NS);
        //if(lo <= 1){
        result = (u0 * u3) >> 6;
        printf("u0=%d \n", u0);
        printf("u3=%d \n", u3);
        printf("result=%d \n", result);
        o_u3.write(result);
            wait(CLOCK_PERIOD, SC_NS);
        //}
        /*else{
            result = 0;
            o_u1.write(result);
            wait(CLOCK_PERIOD, SC_NS);
        }*/
    }
  }
  void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay){
    wait(delay);
    // unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    // auto len = payload.get_data_length();
    tlm::tlm_command cmd = payload.get_command();
    sc_dt::uint64 addr = payload.get_address();
    unsigned char *data_ptr = payload.get_data_ptr();
    //unsigned char *data_ptr2 = payload.get_data_ptr() + 4;
    //unsigned char *data_ptr3 = payload.get_data_ptr() + 8;
    int *f_ptr = (int *)data_ptr;
    //int *f_ptr2 = (int *)data_ptr2;
    //int *f_ptr3 = (int *)data_ptr3;

    addr -= base_offset;


    // cout << (int)data_ptr[0] << endl;
    // cout << (int)data_ptr[1] << endl;
    // cout << (int)data_ptr[2] << endl;
    word buffer;

    switch (cmd) {
      case tlm::TLM_READ_COMMAND:
        // cout << "READ" << endl;
        switch (addr) {
          case PARA3_R_ADDR:
            buffer.sint = o_u3.read();
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
          case PARA3_W1_ADDR:
            i_u0.write(*f_ptr);
            //i_lo.write(*f_ptr3);
            //std::cerr << "WRITE u0";
            break;
          case PARA3_W2_ADDR:
            i_u3.write(*f_ptr);
            //i_lo.write(*f_ptr3);
            //std::cerr << "WRITE u1";
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
//#endif

