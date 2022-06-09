#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
using namespace std;

#include "Testbench.h"

Testbench::Testbench(sc_module_name n) : sc_module(n) {
  SC_THREAD(do_lu);
  sensitive << i_clk.pos();
  dont_initialize();
}

Testbench::~Testbench() {
	//cout<< "Max txn time = " << max_txn_time << endl;
	//cout<< "Min txn time = " << min_txn_time << endl;
	//cout<< "Avg txn time = " << total_txn_time/n_txn << endl;
	cout << "Total run time = " << total_run_time << endl;
}

void Testbench::do_lu() {
  int i, j, x, y;        // for loop counter
  int result1;
  //int result2;
  //sc_dt::sc_int<24> result_int;
  sc_dt::sc_int<32> A[10][10];
  sc_dt::sc_int<32> A_ori[10][10];
  float tmp;
  sc_dt::sc_int<24> tmp_int;

	n_txn = 0;
	max_txn_time = SC_ZERO_TIME;
	min_txn_time = SC_ZERO_TIME;
	total_txn_time = SC_ZERO_TIME;

#ifndef NATIVE_SYSTEMC
  o_u1.reset();
  o_u2.reset();
  //o_u3.reset();
  //o_u4.reset();
  i_result1.reset();
  //i_result2.reset();
#endif
	o_rst.write(false);
	wait(5);
	o_rst.write(true);
	wait(1);
	//total_start_time = sc_time_stamp();
  
  //build initial data
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      A[j][i] = rand() % 50 +1;
      A_ori[j][i] = A[j][i];
    }
  }
  
  //lu decomposition
  for (int i = 0; i < 10; ++i) {
    for (int j = i+1; j < 10; ++j) {
      A[j][i] = A[j][i] << 6;
      A[j][i] /= A[i][i];
    }
    int num;
    int partial;
    
    //num = (20-(i+1)) * (20-(i+1));
    //partial = 20 - (i+1);
    //cout<< num<<endl;
    for (int j = i+1; j < 10; ++j) {
      for (int k = i+1; k < 10; ++k) {
        total_start_time = sc_time_stamp();
#ifndef NATIVE_SYSTEMC
        o_u1.put(A[j][i]);
#else
        o_u1.write(A[j][i]);
#endif
#ifndef NATIVE_SYSTEMC
        o_u2.put(A[i][k]);
#else
        o_u2.write(A[i][k]);
#endif


#ifndef NATIVE_SYSTEMC
        result1 = i_result1.get();
#else
        result1 = i_result1.read();
#endif 
        //cout<< "result1 = " << result1<<endl;
        A[j][k] -= result1;
        total_run_time = sc_time_stamp() - total_start_time;
      }
    }
  
  } 

  //input_file.close();

  ofstream ofs;
  ofs.open("output.txt");
  
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      tmp = A[j][i];
      tmp = tmp / (1 << 6);
      if(!ofs.is_open()){
        cout<<"fail to open file"<<endl;
      }
      else{
        ofs<<tmp<<endl;
      }
    }
  }

  ofs.close();
  
  //total_run_time = sc_time_stamp() - total_start_time;
  sc_stop();
}


