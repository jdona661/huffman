#include <iostream>
#include <fstream>
#include <string>
#include "pq.h"

using namespace std;

int frequencies[256] = {0};

//int* buildFrequencyTable()

void test(string filename) {

	ifstream file(filename.c_str());

	char* ch = new char[1];
	while(file){
		file.read(ch,1);
		frequencies[*ch] += 1;
	}
	file.close();

	for(int i = 0; i<256; i++){
		cout << frequencies[i] << " ";
	}
}


int main(int argc, char* argv[]){
	test("test.txt");
	return 0;
}
