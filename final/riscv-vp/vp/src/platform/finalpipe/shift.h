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

struct shift : public sc_module {
  //tlm_utils::simple_target_socket<para_unit1> tsock;

  sc_fifo_out<int> o_shift;
  sc_fifo_in<int> i_shift;

  SC_HAS_PROCESS(shift);

  shift(sc_module_name n): 
    sc_module(n), 
   // tsock("t_skt"), 
    base_offset(0) 
  {
    //tsock.register_b_transport(this, &para_unit1::do_compute);
    SC_THREAD(do_compute);
  }

  ~shift() {
	}

  int sh;
  int result;
  
  //int done;
  unsigned int base_offset;

  void do_compute(){
      while(true){     
        { wait(CLOCK_PERIOD, SC_NS); }
        sh = i_shift.read();
        //wait(CLOCK_PERIOD, SC_NS);
        result = sh >> 6;
        o_shift.write(result);
        //wait(CLOCK_PERIOD, SC_NS);   
      }    
    }
};
//#endif



