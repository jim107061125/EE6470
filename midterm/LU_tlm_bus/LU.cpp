#include <cmath>
#include <iomanip>

#include "LU.h"

LU::LU(sc_module_name n)
    : sc_module(n), t_skt("t_skt"), base_offset(0) {
  SC_THREAD(do_solve);

  t_skt.register_b_transport(this, &LU::blocking_transport);
}

LU::~LU() = default;

float A[10][10];
float b[10];
float x[10];
float y[10];

void LU::do_solve() {
  int n;
  n = i_n.read();
  //std::cout << n << std::endl;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      A[i][j] = i_a.read();
      //std::cout << A[i][j] << std::endl;
    }
  }

  for (int i = 0; i < n; ++i) {
    b[i] = i_b.read();
  }

  //lu decomposition
  for (int i = 0; i < n; ++i) {
    for (int j = i+1; j < n; ++j) {
      A[j][i] /= A[i][i];
    }

    for (int j = i+1; j < n; ++j) {
      for (int k = i+1; k < n; ++k) {
        A[j][k] -= A[j][i] * A[i][k];
      }
    }
  }



  //forward substitution
  for (int i = 0; i < n; ++i) {
    y[i] = b[i];
  }

  for (int i = 0; i < n; ++i) {
    for (int j = i+1; j < n; ++j) {
      y[j] -= A[j][i] * y[i];
    }
  }

  //backward substitution
  for (int i = 0; i < n; ++i) {
    x[i] = y[i];
  }

  for (int i = n-1; i >= 0; i--) {
    x[i] /= A[i][i];
    for (int j = i-1; j >= 0; j--) {
      x[j] -= A[j][i] * x[i];
    }
  } 

  //write back
  for (int i = 0; i < n; ++i) {
    o_x.write(x[i]);
    //std::cout << x[i] << std::endl;
  }
  
}

void LU::blocking_transport(tlm::tlm_generic_payload &payload,
                                     sc_core::sc_time &delay) {
  sc_dt::uint64 addr = payload.get_address();
  addr -= base_offset;
  unsigned char *mask_ptr = payload.get_byte_enable_ptr();
  unsigned char *data_ptr = payload.get_data_ptr();
  float *f_ptr = (float *)data_ptr;
  word buffer;
  //unsigned char buffer_unc = (unsigned char )buffer;
  switch (payload.get_command()) {
  case tlm::TLM_READ_COMMAND:
    switch (addr) {
    case SOBEL_FILTER_RESULT_ADDR:
      buffer.fd = o_x.read();
      break;
    case SOBEL_FILTER_CHECK_ADDR:
      buffer.fd = o_x.num_available();
    break;
    default:
      std::cerr << "Error! LU::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
    }
    data_ptr[0] = buffer.uc[0];
    data_ptr[1] = buffer.uc[1];
    data_ptr[2] = buffer.uc[2];
    data_ptr[3] = buffer.uc[3];
    break;
  case tlm::TLM_WRITE_COMMAND:
    switch (addr) {
    case SOBEL_FILTER_RN_ADDR:
      if (mask_ptr[0] == 0xff) {
        i_n.write((int)(*data_ptr));
      }
      break;
    case SOBEL_FILTER_RA_ADDR:
      if (mask_ptr[0] == 0xff) {
        i_a.write(*f_ptr);
        //std::cout << *f_ptr << std::endl;
      }
      break;
    case SOBEL_FILTER_RB_ADDR:
      if (mask_ptr[0] == 0xff) {
        i_b.write(*f_ptr);
      }
      break;
    default:
      std::cerr << "Error! LU::blocking_transport: address 0x"
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