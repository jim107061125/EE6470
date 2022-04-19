#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

class LU : public sc_module {
public:
  tlm_utils::simple_target_socket<LU> t_skt;

  sc_fifo<unsigned int> i_n;
  sc_fifo<float> i_a;
  sc_fifo<float> i_b;
  sc_fifo<float> o_x;

  SC_HAS_PROCESS(LU);
  LU(sc_module_name n);
  ~LU();

private:
  void do_solve();

  unsigned int base_offset;
  void blocking_transport(tlm::tlm_generic_payload &payload,
                          sc_core::sc_time &delay);
};
#endif
