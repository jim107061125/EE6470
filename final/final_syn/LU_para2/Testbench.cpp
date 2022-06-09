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
  int result2;
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
  o_u3.reset();
  o_u4.reset();
  i_result1.reset();
  i_result2.reset();
#endif
	o_rst.write(false);
	wait(5);
	o_rst.write(true);
	wait(1);
	total_start_time = sc_time_stamp();

  /*char mdat[] = "m4.dat";

  ifstream input_file(mdat);
  if (!input_file.is_open()) {
    cerr << "Could not open the file - '"
      << mdat << "'" << endl;
      return;
  }

  input_file >> n;*/
  
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
    
    num = (10-(i+1)) * (10-(i+1));
    partial = 10 - (i+1);
    //cout<< num<<endl;
    for (int j = 0; j < num; j=j+2) {
        //total_start_time = sc_time_stamp();
        int r1 = j / partial + (i+1);
		    int c1 = j % partial + (i+1);
		    int r2 = (j+1) / partial + (i+1);
		    int c2 = (j+1) % partial + (i+1);
        //cout<< r1 << c1<<endl;
        //cout<< r2 << c2<<endl;
        //cout<< j<<endl;
#ifndef NATIVE_SYSTEMC
        o_u1.put(A[r1][i]);
#else
        o_u1.write(A[r1][i]);
#endif
#ifndef NATIVE_SYSTEMC
        o_u2.put(A[i][c1]);
#else
        o_u2.write(A[i][c1]);
#endif
        if((j+1) < num){
#ifndef NATIVE_SYSTEMC
          o_u3.put(A[r2][i]);
#else
          o_u3.write(A[r2][i]);
#endif
#ifndef NATIVE_SYSTEMC
          o_u4.put(A[i][c2]);
#else
          o_u4.write(A[i][c2]);
#endif                
        }

#ifndef NATIVE_SYSTEMC
        result1 = i_result1.get();
#else
        result1 = i_result1.read();
#endif 
        //cout<< "result1 = " << result1<<endl;
        A[r1][c1] -= result1;
        
        if((j+1) < num){
#ifndef NATIVE_SYSTEMC
          result2 = i_result2.get();
#else
          result2 = i_result2.read();
#endif
          //cout<< "result2 = " << result2<<endl;
          A[r2][c2] -= result2;           
        }
        //total_run_time = sc_time_stamp() - total_start_time;
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
  
  total_run_time = sc_time_stamp() - total_start_time;
  sc_stop();
}


