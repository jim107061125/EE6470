#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;

#include "Testbench.h"

Testbench::Testbench(sc_module_name n)
    : sc_module(n), initiator("initiator") {
  SC_THREAD(do_lu);
}

Testbench::~Testbench() {}

void Testbench::do_lu() {
  int i, j, x, y;        // for loop counter
  float result;
  int n;
  float tmp;

  word data;
  unsigned char mask[4];
  //wait(5 * CLOCK_PERIOD, SC_NS);

  string filename("m4.dat");

  ifstream input_file(filename);
  if (!input_file.is_open()) {
    cerr << "Could not open the file - '"
      << filename << "'" << endl;
      return;
  }

  input_file >> n;
  data.uint = n;
  mask[0] = 0xff;
  mask[1] = 0xff;
  mask[2] = 0xff;
  mask[3] = 0xff;
  initiator.write_to_socket(SOBEL_MM_BASE + SOBEL_FILTER_RN_ADDR, mask,
                            data.uc, 4);
  wait(1 * CLOCK_PERIOD, SC_NS);

  for (y = 0; y != n; ++y) {
    for (x = 0; x != n; ++x) {
      input_file >> tmp;
      data.fd = tmp;
      mask[0] = 0xff;
      mask[1] = 0xff;
      mask[2] = 0xff;
      mask[3] = 0xff;
      initiator.write_to_socket(SOBEL_MM_BASE + SOBEL_FILTER_RA_ADDR, mask,
                                data.uc, 4);
      wait(1 * CLOCK_PERIOD, SC_NS);
    }
  }

  for (i = 0; i != n; ++i) {
    input_file >> tmp;
    data.fd = tmp;
    mask[0] = 0xff;
    mask[1] = 0xff;
    mask[2] = 0xff;
    mask[3] = 0xff;
    initiator.write_to_socket(SOBEL_MM_BASE + SOBEL_FILTER_RB_ADDR, mask,
                              data.uc, 4);
    wait(1 * CLOCK_PERIOD, SC_NS);
  }
  input_file.close();

  ofstream ofs;
  ofs.open("output.txt");

  for (j = 0; j != n; ++j){
    bool done=false;
    int output_num=0;
    while(!done){
      initiator.read_from_socket(SOBEL_MM_BASE + SOBEL_FILTER_CHECK_ADDR, mask, data.uc, 4);
      output_num = data.sint;
      if(output_num>0) done=true;
    }
    wait(1 * CLOCK_PERIOD, SC_NS);

    initiator.read_from_socket(SOBEL_MM_BASE + SOBEL_FILTER_RESULT_ADDR, mask,
                                data.uc, 4);
    result = data.fd;

    if(!ofs.is_open()){
      cout<<"fail to open file"<<endl;
    }
    else{
      ofs<<result<<endl;
    }
  }
  ofs.close();

  sc_stop();
}
