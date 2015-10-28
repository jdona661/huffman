#include <iostream>
#include <fstream>
#include <string>
#include "pq.h"
#include "huffman.h"

using namespace std;

int counts[256] = {0};



struct HuffmanNode {
	char character;
	unsigned int weight;
	
	HuffmanNode(char ch, unsigned int w): character(ch), weight(w) {}

	bool operator<(HuffmanNode const& rhs) const {
		if(this->weight < rhs.weight){
			return true;
		}else{
			return false;
		}
	}
};




Huffman::Huffman(int* frequencies){
	PQ<HuffmanNode> heap = PQ<HuffmanNode>(2);	

	//build initial minheap
	for(char ch = 0; ch < 256; ch++){
		if(frequencies[ch] != 0){
			HuffmanNode node = HuffmanNode(ch,frequencies[ch]);
			heap.push(node);
		}
	}
	
	//Extract two minimum nodes
	while(heap.size() > 1){
		HuffmanNode left = heap.top();
		heap.pop();
		HuffmanNode right = heap.top();
		heap.pop();
	}	
}

Huffman Huffman::buildTreeFromFile(const char *filename){
	int counts[256] = {0};
	Huffman huffman;
	ifstream file(filename);
	char* ch = new char[1];
	while(file){
		file.read(ch,1);
		counts[*ch] += 1;
	}

	huffman = Huffman(counts);
	return huffman;
	
}


int main(int argc, char* argv[]){

	if(argc != 2){
		cout << "proper usage: huffman [file_to_compress]" << endl;
		return 0;
	}

	
	Huffman huffman;
	huffman = Huffman::buildTreeFromFile(argv[1]);
	return 0;
}
