//#ifndef SOBEL_FILTER_H_
//#define SOBEL_FILTER_H_
#include <systemc>
#include <cmath>
#include <iomanip>
#include <stdio.h>
using namespace sc_core;

#include <tlm>
//#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

struct para_unit4 : public sc_module {
  //tlm_utils::simple_target_socket<LU> tsock;

  sc_fifo_out<int> o_u4;
  sc_fifo_in<int> i_u0;
  sc_fifo_in<int> i_u4;
  sc_fifo_in<int> i_lo;
  //sc_fifo_in<int> i_up;

  SC_HAS_PROCESS(para_unit4);

  para_unit4(sc_module_name n): 
    sc_module(n), 
    //tsock("t_skt"), 
    base_offset(0) 
  {
    //tsock.register_b_transport(this, &LU::blocking_transport);
    SC_THREAD(do_compute);
  }

  ~para_unit4() {
	}

  int u0, u4;
  int lo;
  int result;
  
  //int done;
  unsigned int base_offset;

  void do_compute(){
    while(true){    
        { wait(CLOCK_PERIOD, SC_NS); }

        lo = i_lo.read();
        u0 = i_u0.read();
        u4 = i_u4.read();
        wait(CLOCK_PERIOD, SC_NS);
        if(lo <= 4){
            result = (u0 * u4) >> 6;
            o_u4.write(result);
            wait(CLOCK_PERIOD, SC_NS);
        }
        else{
            result = 0;
            o_u4.write(result);
            wait(CLOCK_PERIOD, SC_NS);
        }
    }
  }
};
//#endif

