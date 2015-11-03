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


    //consume bits from the input string until a match is found;
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




Huffman::Huffman(const int* _frequencies){
    PQ<HuffmanNode> heap = PQ<HuffmanNode>(2);
    frequencies = _frequencies;


    //build initial minheap
    for(int ch = 0; ch < 256; ch += 1){
        if(frequencies[ch] != 0){
            heap.push(HuffmanNode(ch,frequencies[ch]));
        }
    }

    //Extract two minimum nodes and create a new one till there's only one left
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
    int *counts;
    counts = new int[256];
    fill_n(counts,256,0);

    Huffman huffman;
    ifstream file(filename);
    if(file.fail()){
        throw exception();
    }

    char ch;
    while(file){
        file.get(ch);
        counts[ch] += 1;
    }
    file.close();

    huffman = Huffman(counts);
    return huffman;

}

Huffman Huffman::readTreeFromFile(const char *filename){
    ifstream sfile(filename);
    if(sfile.fail()){
    	throw exception();
    }

    //reconstruct the frequency table from the file 
    int count;
    int *counts = new int[256];
    fill_n(counts,256,0);

    for(int i = 0; i < 256; i++){
        sfile >> count;
        counts[i] = count;
    }
    sfile.close();

    Huffman huffman = Huffman(counts);
    return huffman;
}


//output frequency table to file.  one line per entry 
void Huffman::saveTreeToFile(const char *filename){
    ofstream dfile(filename);

    for(int ch = 0; ch < 256; ch += 1){
        dfile << frequencies[ch] << endl;
    }

    dfile.close();
}

void Huffman::compress(const char *source, const char *dest){
    string intermediate;
    int paddingSize;
    ifstream sfile(source,ios::binary);
    if(sfile.fail()){
    	throw exception();
    }


    ofstream dfile(dest);

    char ch;
    //create string with codes
    while(sfile.get(ch)){
        intermediate += root->encode(ch);
    }

    //calculate required padding
    if (intermediate.length() % 8 == 0){				
        paddingSize = 0;
    }else{
        paddingSize = 8 - (intermediate.length() % 8);
    }

    intermediate.insert(0,paddingSize,'0');				//prepend padding to intermediate
    dfile << (char)paddingSize;							//insert padding amount byte to file

    for(int i = 0; i < intermediate.length(); i += 8){
        string byteString = intermediate.substr(i,8);
        bitset<8> bits;
        bits.set();
        for(int j = 0; j < 8; ++j){
            if(byteString[j] == '0') { bits.set((8-j-1)%8,0); }	 //set the bits backwards 	
        }
        dfile << char(bits.to_ulong());
    }
    dfile.close();
}


void Huffman::decompress(const char *source, const char *dest){
    string bytes;
    string bits;
    char paddingSize;
    ifstream sfile(source,ios::binary);
    if(sfile.fail()){
    	throw exception();
    }

    ofstream dfile(dest);

    sfile.get(paddingSize);

    char ch;
    while(sfile.get(ch)){
        bytes += ch;
    }
    sfile.close();

    for(char ch: bytes){
        bits += bitset<8>(ch).to_string();
    }
    bits.erase(bits.begin(),bits.begin() + (int)paddingSize);	//remove padding

    while(bits.length() > 1){
        char c = root->decode(bits);
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
    	try{
	        Huffman huffman;
	        huffman = Huffman::buildTreeFromFile(argv[2]);
	        huffman.compress(argv[2], argv[3]);
	        huffman.saveTreeToFile(".tree");
	        cout << "successfully compressed " << argv[2] << " to " << argv[3] << endl;
	    }catch(exception& e){
	    	cout << "compression failed.  please ensure that the filenames are correct" << endl;
	    }    

        return 0;
    }else if(string(argv[1]) == "-d"){
    	try{
	        Huffman huffman;
	        huffman = Huffman::readTreeFromFile(".tree");
	        huffman.decompress(argv[2],argv[3]);
	        cout << "successfully decompressed " << argv[2] << " to " << argv[3] << endl;
	        return 0;
	    }catch(exception& e){
	    	cout << "decompression failed.  please ensure that the filenames are correct" << endl;
	    }    
    }else{
        cout << usage;
        return 1;
    }
}
