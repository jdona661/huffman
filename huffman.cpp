#include <iostream>
#include <fstream>
#include <string>
#include <bitset>

#include "pq.h"
#include "huffman.h"

using namespace std;

struct HuffmanNode {
	char ch;
	unsigned int weight;
	HuffmanNode* left;
	HuffmanNode* right;
	
	HuffmanNode(char c, unsigned int w): ch(c), weight(w), left(nullptr), right(nullptr) { ; }
	HuffmanNode(char c, unsigned int w, HuffmanNode* l, HuffmanNode* r): ch(c), weight(w), left(l), right(r) { ; }

	bool isLeaf(){
		return (left==nullptr && right==nullptr);
	}

	string encodeChar(char c, string code="") {
        string encoding = "";
        if (isLeaf() && ch == c) return code;
        if (nullptr != left) {
            encoding += left->encodeChar(c, code + "0");
        }
        if (nullptr != right) {
            encoding += right->encodeChar(c, code + "1");
        }
        return encoding;
    }


	bool operator<(HuffmanNode const& rhs) const {
		return this->weight > rhs.weight;		//why this works, I don't know 
	}

};




Huffman::Huffman(const int* frequencies){
	PQ<HuffmanNode> heap = PQ<HuffmanNode>(2);	

	//build initial minheap
	for(int ch = 0; ch < 256; ch += 1){
		if(frequencies[ch] != 0){
			heap.push(HuffmanNode(ch,frequencies[ch]));
			//cout << "pushed " << (char)ch << " onto the heap" << endl; 
		}
	}
	
	//Extract two minimum nodes and create new one till they're all gone
	while(heap.size() > 1){

		HuffmanNode* left = new HuffmanNode(heap.top().ch,heap.top().weight,heap.top().left, heap.top().right);
		heap.pop();
		HuffmanNode* right = new HuffmanNode(heap.top().ch,heap.top().weight,heap.top().left, heap.top().right);
		heap.pop();

		cout << "popped " << left->ch << " and " << right->ch << endl;
		cout << "stack size: " << heap.size() << endl; 		

		HuffmanNode node =  HuffmanNode('$', left->weight + right->weight, left, right);
		heap.push(node);
	}

	root = new HuffmanNode(heap.top().ch,heap.top().weight,heap.top().left, heap.top().right);
}

Huffman Huffman::buildTreeFromFile(const char *filename){
	int counts[256] = {0};
	Huffman huffman;
	ifstream file(filename);
	char ch;
	while(file){
		file.get(ch);
		counts[ch] += 1;
	}
	file.close();

	huffman = Huffman(counts);
	return huffman;
	
}

void Huffman::compress(const char *source, const char *dest){
	string intermediate;
	bitset<8> byte;

	ifstream sfile(source);
	ofstream dfile(dest);

	char ch;
	//create string with codes 
	while(sfile){
		sfile.get(ch);
		cout << ch << " " << root->encodeChar(ch) << endl;
		intermediate += root->encodeChar(ch);
	}
	cout << intermediate << endl;
	int paddingSize = 8 - (intermediate.length() % 8);

	intermediate.insert(0,paddingSize,'0'); 
	char paddingSizeByte = (char)paddingSize;



	//dfile << paddingSizeByte;

	cout << paddingSize << " padding bits required" << endl; 
	cout << "padding byte: " << paddingSizeByte << endl;

	cout << intermediate << endl;

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
	huffman.compress(argv[1], "asef");
	return 0;
}
