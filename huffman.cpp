#include <iostream>
#include <fstream>
#include <string>
#include "pq.h"
#include "huffman.h"

using namespace std;

int counts[256] = {0};



struct HuffmanNode {
	char ch;
	unsigned int weight;
	HuffmanNode* left;
	HuffmanNode* right;
	
	HuffmanNode(char c, unsigned int w): ch(c), weight(w), left(nullptr), right(nullptr) { ; }
	HuffmanNode(char c, unsigned int w, HuffmanNode* l, HuffmanNode* r): ch(c), weight(w), left(l), right(r) { ; }
	bool isLeaf(){
		return (left==nullptr and right==nullptr);
	}




	bool operator<(HuffmanNode const& rhs) const {
		return this->weight < rhs.weight;
	}
};




Huffman::Huffman(const int* frequencies){
	PQ<HuffmanNode> heap = PQ<HuffmanNode>(2);	

	//build initial minheap
	for(char ch = 0; ch < 256; ch++){
		if(frequencies[ch] != 0){
			heap.push(HuffmanNode(ch,frequencies[ch]));
		}
	}
	
	//Extract two minimum nodes
	while(heap.size() > 1){
		HuffmanNode* left = &(heap.top());
		heap.pop();
		HuffmanNode* right = &(heap.top());
		heap.pop();

		
		HuffmanNode node = HuffmanNode('$', left->weight + right->weight, left, right);
		heap.push(node);
	}

	root = heap.top();


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

Huffman::~Huffman(){
	return;
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
