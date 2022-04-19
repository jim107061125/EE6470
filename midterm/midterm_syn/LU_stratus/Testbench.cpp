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
  float result;
  sc_dt::sc_int<24> result_int;
  sc_dt::sc_int<5> n;
  float tmp;
  sc_dt::sc_int<24> tmp_int;

	n_txn = 0;
	max_txn_time = SC_ZERO_TIME;
	min_txn_time = SC_ZERO_TIME;
	total_txn_time = SC_ZERO_TIME;

#ifndef NATIVE_SYSTEMC
	o_n.reset();
  o_a.reset();
  o_b.reset();
  i_x.reset();
#endif
	o_rst.write(false);
	wait(5);
	o_rst.write(true);
	wait(1);
	total_start_time = sc_time_stamp();

  char mdat[] = "m4.dat";
  //string filename(mdat);

  ifstream input_file(mdat);
  if (!input_file.is_open()) {
    cerr << "Could not open the file - '"
      << mdat << "'" << endl;
      return;
  }

  input_file >> n;
#ifndef NATIVE_SYSTEMC
  o_n.put(n);
#else
  o_n.write(n);
#endif  

  for (y = 0; y != n; ++y) {
    for (x = 0; x != n; ++x) {
      input_file >> tmp;
      tmp = tmp * (1 << 6);
#ifndef NATIVE_SYSTEMC
      o_a.put(tmp);
#else
      o_a.write(tmp);
#endif 
    }
  }

  for (i = 0; i != n; ++i) {
    input_file >> tmp;    
    tmp_int = tmp * (1 << 10);
#ifndef NATIVE_SYSTEMC
    o_b.put(tmp_int);
#else
    o_b.write(tmp_int);
#endif
  }
  input_file.close();

  ofstream ofs;
  ofs.open("output.txt");

  for (j = 0; j != n; ++j){
#ifndef NATIVE_SYSTEMC
    result_int = i_x.get();
#else
    result_int = i_x.read();
#endif
    result = result_int;
    result = result / (1 << 10);
    //cout<<result_int<<endl;
    if(!ofs.is_open()){
      cout<<"fail to open file"<<endl;
    }
    else{
      ofs<<result<<endl;
    }
  }
  ofs.close();
  
  total_run_time = sc_time_stamp() - total_start_time;
  sc_stop();
}


