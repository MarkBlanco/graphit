#include <stdio.h>
#include<stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

float gen_rand_float(){
  return float(( rand()*1.0) /(RAND_MAX));
}

int * load_sources(std::string filename, int num_nodes){
	int * sources = (int*)malloc(sizeof(int)*num_nodes);
	std::ifstream infile;
	infile.open( filename, std::fstream::in);
	int total_nodes=0;
	while (infile.good() && total_nodes < num_nodes){
		int tmp;
		infile >> tmp;
		sources[total_nodes] = tmp;
		//std::cout << "Added " << tmp << std::endl;
		total_nodes++;
	}
	infile.close();
	return sources;
}
