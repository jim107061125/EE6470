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
  int result3;
  int result4;
  int k2;
  int k3;
  int k4;
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
  o_u5.reset();
  o_u6.reset();
  o_u7.reset();
  o_u8.reset();
  i_result1.reset();
  i_result2.reset();
  i_result3.reset();
  i_result4.reset();
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
    /*for (int j = i+1; j < 10; ++j) {
      for (int k = i+1; k < 10; k=k+4) {
        k2 = k+1;
        k3 = k+2;
        k4 = k+3;
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
        if(k2 < 10){
#ifndef NATIVE_SYSTEMC
        o_u3.put(A[j][i]);
#else
        o_u3.write(A[j][i]);
#endif
#ifndef NATIVE_SYSTEMC
        o_u4.put(A[i][k2]);
#else
        o_u4.write(A[i][k2]);
#endif
        }
        if(k3 < 10){
#ifndef NATIVE_SYSTEMC
        o_u5.put(A[j][i]);
#else
        o_u5.write(A[j][i]);
#endif
#ifndef NATIVE_SYSTEMC
        o_u6.put(A[i][k3]);
#else
        o_u6.write(A[i][k3]);
#endif
        }
        if(k4 < 10){
#ifndef NATIVE_SYSTEMC
        o_u7.put(A[j][i]);
#else
        o_u7.write(A[j][i]);
#endif
#ifndef NATIVE_SYSTEMC
        o_u8.put(A[i][k3]);
#else
        o_u8.write(A[i][k3]);
#endif
        }

#ifndef NATIVE_SYSTEMC
        result1 = i_result1.get();
#else
        result1 = i_result1.read();
#endif 
        //cout<< "result1 = " << result1<<endl;
        A[j][k] -= result1;
        
        if(k2 < 10){
#ifndef NATIVE_SYSTEMC
          result2 = i_result2.get();
#else
          result2 = i_result2.read();
#endif 
        //cout<< "result1 = " << result1<<endl;
          A[j][k2] -= result2;        
        }
        if(k3 < 10){
#ifndef NATIVE_SYSTEMC
          result3 = i_result3.get();
#else
          result3 = i_result3.read();
#endif 
        //cout<< "result1 = " << result1<<endl;
          A[j][k3] -= result3;        
        }
        if(k4 < 10){
#ifndef NATIVE_SYSTEMC
          result4 = i_result4.get();
#else
          result4 = i_result4.read();
#endif 
        //cout<< "result1 = " << result1<<endl;
          A[j][k4] -= result4;        
        }
      }
    }*/
    
    num = (10-(i+1)) * (10-(i+1));
    partial = 10 - (i+1);
    //cout<< "num ="<<num<<endl;
    for (int j = 0; j < num; j=j+4) {
        int r1 = j / partial + (i+1);
		    int c1 = j % partial + (i+1);
		    int r2 = (j+1) / partial + (i+1);
		    int c2 = (j+1) % partial + (i+1);
        int r3 = (j+2) / partial + (i+1);
		    int c3 = (j+2) % partial + (i+1);
        int r4 = (j+3) / partial + (i+1);
		    int c4 = (j+3) % partial + (i+1);
        //cout<< r4 << c4<<endl;
        //cout<< r2 << c2<<endl;
        cout<< j<<endl;
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
        
        if((j+2) < num){
#ifndef NATIVE_SYSTEMC
          o_u5.put(A[r3][i]);
#else
          o_u5.write(A[r3][i]);
#endif
#ifndef NATIVE_SYSTEMC
          o_u6.put(A[i][c3]);
#else
          o_u6.write(A[i][c3]);
#endif                
        }
        if((j+3) < num){
#ifndef NATIVE_SYSTEMC
          o_u7.put(A[r4][i]);
#else
          o_u7.write(A[r4][i]);
#endif
#ifndef NATIVE_SYSTEMC
          o_u8.put(A[i][c4]);
#else
          o_u8.write(A[i][c4]);
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
        if((j+2) < num){
#ifndef NATIVE_SYSTEMC
          result3 = i_result3.get();
#else
          result3 = i_result3.read();
#endif
          //cout<< "result2 = " << result2<<endl;
          A[r3][c3] -= result3;           
        }
        if((j+3) < num){
#ifndef NATIVE_SYSTEMC
          result4 = i_result4.get();
#else
          result4 = i_result4.read();
#endif
          //cout<< "result2 = " << result2<<endl;
          A[r4][c4] -= result4;           
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


