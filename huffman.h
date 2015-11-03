#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <unordered_map>


struct HuffmanNode;
class Huffman {
	HuffmanNode *root;
	const int* frequencies;
	Huffman(const int* _frequencies);

public:	
	Huffman() : root(nullptr) { ; }
	Huffman(HuffmanNode *r) : root(r) { ; }
	~Huffman();
	static Huffman buildTreeFromFile(const char *filename);
	static Huffman readTreeFromFile(const char *filename);

	// the following method throw Exception objects upon failure
	void saveTreeToFile(const char *filename);
	void compress(const char *source, const char *dest);
	void decompress(const char *source, const char *dest);

	static void runUnitTests();

#ifdef TESTING
	void print() const;
	static void runUnitTests();
#endif
};

#endif
