//#ifndef SOBEL_FILTER_H_
//#define SOBEL_FILTER_H_
#include <systemc>
#include <cmath>
#include <iomanip>
#include <stdio.h>
using namespace sc_core;

//#include <tlm>
//#include <tlm_utils/simple_target_socket.h>

//#include "filter_def.h"

struct mul : public sc_module {
  //tlm_utils::simple_target_socket<para_unit1> tsock;

  sc_fifo_out<int> o_mul;
  sc_fifo_in<int> i_mul1;
  sc_fifo_in<int> i_mul2;
  //sc_fifo_in<int> i_up;

  SC_HAS_PROCESS(mul);

  mul(sc_module_name n): 
    sc_module(n), 
   // tsock("t_skt"), 
    base_offset(0) 
  {
    //tsock.register_b_transport(this, &para_unit1::do_compute);
    SC_THREAD(do_compute);
  }

  ~mul() {
	}

  int mul1, mul2;
  int result;
  
  //int done;
  unsigned int base_offset;

  void do_compute(){
      while(true){      
        { wait(CLOCK_PERIOD, SC_NS); }
        mul1 = i_mul1.read();
        mul2 = i_mul2.read();
        //wait(CLOCK_PERIOD, SC_NS);
        result = mul1 * mul2;
        o_mul.write(result);
        //wait(CLOCK_PERIOD, SC_NS);  
      }     
    }
};
//#endif


