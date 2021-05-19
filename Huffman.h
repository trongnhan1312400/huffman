#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

typedef unsigned char unchar;
typedef unsigned long unlong;
typedef unsigned int uint;

class Frequency
{
private :

	unlong *_F;

public :

	Frequency();
	~Frequency();

	void setFrequency(const string &data);

	void setFrequency(const vector<unlong> &F);

	void clear();

	unlong getFrequency(unchar index);
};

struct HuffNode
{
	string data;
	unlong f;
	
	HuffNode* left;
	HuffNode* right;
};

class HuffmanTree
{
private :
	
	HuffNode* _root;

	void clean(HuffNode *root);

	void findMinPosition(vector<HuffNode*> &tree);

	bool find(char character, string &s);

public :

	HuffmanTree();
	~HuffmanTree();

	vector<vector<char>> detectCode();
	
	void buildTree(Frequency &F);

	HuffNode* getRoot();
};

class Huffman
{
public :
	
	string encode(string &data);

	string decode(string &data);
};