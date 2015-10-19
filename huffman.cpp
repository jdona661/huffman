#include <iostream>
#include <fstream>
#include <string>
#include "pq.h"

using namespace std;

int frequencies[256] = {0};
PQ<int> tree = PQ<int>(2);

int* buildFrequencyTable(string filename){
	ifstream file(filename.c_str());

	char* ch = new char[1];
	while(file){
		file.read(ch,1);
		frequencies[*ch] += 1;
	}
	file.close();
}


int main(int argc, char* argv[]){
	
	if (argc != 2){
		cout << "Usage: huffman [filename]" << endl;
		return 1;
	}
	
	buildFrequencyTable(string(argv[1]));
	
	for(int i = 0; i<256; i++){
		cout << frequencies[i] << " ";
	}
	
	return 0;
}
