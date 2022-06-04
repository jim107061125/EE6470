#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
#include <cmath>
#include <iomanip>
#include <stdio.h>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

struct LU : public sc_module {
  tlm_utils::simple_target_socket<LU> tsock;

  sc_fifo<unsigned int> i_n;
  sc_fifo<int> i_a;
  sc_fifo<int> i_b;
  sc_fifo<int> o_x;
  sc_fifo<int> o_done;

  SC_HAS_PROCESS(LU);

  LU(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &LU::blocking_transport);
    SC_THREAD(do_lu);
  }

  ~LU() {
	}

  int A[10][10];
  int b[10];
  int x[10];
  int y[10];
  //int done;
  unsigned int base_offset;

  void do_lu(){
    { wait(CLOCK_PERIOD, SC_NS); }
    int n;
    int done;
    n = i_n.read();
    wait(CLOCK_PERIOD, SC_NS);
    //printf("%d\n", n);

    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        A[i][j] = i_a.read();
        //printf("%d\n", A[i][j]);
        wait(CLOCK_PERIOD, SC_NS);
      }
    }

    for (int i = 0; i < n; ++i) {
      b[i] = i_b.read();
      //printf("%d\n", b[i]);
      wait(CLOCK_PERIOD, SC_NS);
    }

    //lu decomposition
    for (int i = 0; i < n; ++i) {
      for (int j = i+1; j < n; ++j) {
        A[j][i] = A[j][i] << 6;
        A[j][i] /= A[i][i];
        wait(CLOCK_PERIOD, SC_NS);
      }

      for (int j = i+1; j < n; ++j) {
        for (int k = i+1; k < n; ++k) {
          A[j][k] -= (A[j][i] * A[i][k]) >> 6;
          wait(CLOCK_PERIOD, SC_NS);
        }
        //printf("%d\n", A[j][5]);
      }
    }

    //forward substitution
    for (int i = 0; i < n; ++i) {
      y[i] = b[i];
      //printf("%d\n", y[i]);
      wait(CLOCK_PERIOD, SC_NS);
    }

    for (int i = 0; i < n; ++i) {
      for (int j = i+1; j < n; ++j) {
        y[j] -= (A[j][i] * y[i]) >> 6;
        wait(CLOCK_PERIOD, SC_NS);
      }
    }

    //backward substitution
    for (int i = 0; i < n; ++i) {
      x[i] = y[i];
      wait(CLOCK_PERIOD, SC_NS);
    }

    for (int i = n-1; i >= 0; i--) {
      x[i] = x[i] << 6;
      x[i] /= A[i][i];
      wait(CLOCK_PERIOD, SC_NS);
      for (int j = i-1; j >= 0; j--) {
        x[j] -= (A[j][i] * x[i]) >> 6;
        wait(CLOCK_PERIOD, SC_NS);
      }
    } 
    done = 1;
    o_done.write(done);
    o_done.write(done);

    //write back
    for (int i = 0; i < n; ++i) {
      o_x.write(x[i]);
      printf("%d\n", x[i]);
      wait(CLOCK_PERIOD, SC_NS);
      //std::cout << x[i] << std::endl;
    }
    /*done = 1;
    o_done.write(done);*/
  }

  void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay){
    wait(delay);
    // unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    // auto len = payload.get_data_length();
    tlm::tlm_command cmd = payload.get_command();
    sc_dt::uint64 addr = payload.get_address();
    unsigned char *data_ptr = payload.get_data_ptr();
    int *f_ptr = (int *)data_ptr;

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
            buffer.sint = o_x.read();
            break;
          case SOBEL_FILTER_DONE_ADDR:
            buffer.sint = o_done.read();
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
          case SOBEL_FILTER_N_ADDR:
            i_n.write((int)(*data_ptr));
            //std::cerr << "WRITE n";
            break;
          case SOBEL_FILTER_A_ADDR:
            i_a.write(*f_ptr);
            break;
          case SOBEL_FILTER_B_ADDR:
            i_b.write(*f_ptr);
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
