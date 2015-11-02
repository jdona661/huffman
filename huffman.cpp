#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <cassert>

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

	string encode(char c, string code="") {
        string encoding = "";
        if (isLeaf() && ch == c) return code;
        if (nullptr != left) {
            encoding += left->encode(c, code + "0");
        }
        if (nullptr != right) {
            encoding += right->encode(c, code + "1");
        }
        return encoding;
    }

    char decode(string& code){
    	if(isLeaf()){
    		return ch;
    	}

    	char dir = code[0];
    	code.erase(code.begin(),code.begin()+1);
    	if(dir == '0'){
    		return left->decode(code);
    	}else if(dir == '1'){
    		return right->decode(code);
    	}	
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
	cout << "= = = = = = = = = = COMPRESSING = = = = = = = = = =" << endl;
	string intermediate;
	int paddingSize;
	ifstream sfile(source,ios::binary);
	ofstream dfile(dest);

	char ch;
	//create string with codes 
	while(sfile.get(ch)){
		cout << ch << " " << root->encode(ch) << endl;
		intermediate += root->encode(ch);
	}

	if (intermediate.length() % 8 == 0){				//calculate required padding
		paddingSize = 0;
	}else{
		paddingSize = 8 - (intermediate.length() % 8);
	}
	
	cout << "paddingSize: " << paddingSize << endl;		
	intermediate.insert(0,paddingSize,'0');				//prepend padding to intermediate
	dfile << (char)paddingSize;							//insert padding amount byte to file
	cout << "intermediate " << intermediate << endl;

	for(int i = 0; i < intermediate.length(); i += 8){
		string byteString = intermediate.substr(i,8);
		cout << byteString << endl;
		bitset<8> bits;
		bits.set();
		for(int j = 0; j < 8; ++j){
			if(byteString[j] == '0') { bits.set((8-j-1)%8,0); }
		}
		cout << "bits: " <<  bits << endl;
		dfile << char(bits.to_ulong());
	}
	cout << intermediate << endl;
}

void Huffman::decompress(const char *source, const char *dest){
	cout << "= = = = = = = = = = DECOMPRESSING = = = = = = = = = =" << endl;
	string bytes;
	string bits;
	char paddingSize;
	ifstream sfile(source,ios::binary);
	ofstream dfile(dest);

	sfile.get(paddingSize);

	char ch;
	while(sfile.get(ch)){
		bytes += ch;
	}
	sfile.close();

	cout << "bytes: " <<  bytes << endl;
	cout << "total bytes: " << bytes.length() << endl << endl;

	for(char ch: bytes){
		bits += bitset<8>(ch).to_string();
	}
	cout << "bits with padding: " << bits << endl;
	cout << "padding size: " << (int)paddingSize << endl;
	bits.erase(bits.begin(),bits.begin() + (int)paddingSize);	
	cout << "bits without padding: " << bits << endl;

	while(bits.length() > 1){
		cout << bits << endl;
		char c = root->decode(bits);
		cout << c << endl;
		dfile << c;
	}
	dfile.close();
}



Huffman::~Huffman(){
	return;
}

int main(int argc, char* argv[]){
	string usage = "proper usage: huffman -[c,d] [source] [dest]\n-c: compress source file to destination\n-d: decompress source file to destination\n";

	if(argc != 4){
		cout << usage;
		return 1;
	}

	if(string(argv[1]) == "-c"){
		Huffman huffman;
		huffman = Huffman::buildTreeFromFile(argv[2]);
		huffman.compress(argv[2], argv[3]);

		cout << "testing . . . " << endl;
		huffman.decompress(argv[3], "final.txt");
		return 0;
	}else{
		cout << usage;
		return 1;
	}

	return 0;
}
