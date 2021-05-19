#pragma once

#include "Huffman.h"

#define MIN_FILESIZE 1026

Frequency
	::Frequency()
{
	_F = new unlong[256];

	for (int i = 0; i < 256; i++)
	{
		_F[i] = 0;
	}
}

Frequency
	::~Frequency()
{
	delete []_F;
}

unlong Frequency
	::getFrequency(unchar index)
{
	return _F[index];
}

void Frequency
	::setFrequency(const string &data)
{
	unchar* Data = (unchar*)data.c_str();
	unlong length = data.length();
	clear();

	for (unlong i = 0; i < length; i++)
	{
		_F[Data[i]]++;
	}
}

void Frequency
	::setFrequency(const vector<unlong> &F)
{
	clear();

	unlong n = F.size();
	if (n > 256)
	{
		n = 256;
	}

	for (int i = 0; i < n; i++)
	{
		_F[i] = F[i];
	}
}

void Frequency
	::clear()
{
	for (int i = 0; i < 256; i++)
	{
		_F[i] = 0;
	}
}

HuffmanTree
	::~HuffmanTree()
{
	clean(_root);
}

HuffmanTree
	::HuffmanTree()
{
	_root = NULL;
}

void HuffmanTree
	::clean(HuffNode* root)
{
	if (root == NULL)
	{
		return;
	}

	clean(root->left);
	clean(root->right);

	delete root;
}

void HuffmanTree
	::findMinPosition(vector<HuffNode*> &tree)
{
	int pos = 0;
	int tsize = tree.size();

	for (int i = 1; i < tsize; i++)
	{
		if (tree[i]->f < tree[pos]->f)
		{
			pos = i;
		}
	}

	HuffNode* t = tree[pos];
	tree[pos] = tree[tsize - 1];
	tree[tsize - 1] = t;
}

void HuffmanTree
	::buildTree(Frequency &F)
{
	vector<HuffNode*> tree;
	tree.resize(256, NULL);

	for (int i = 0; i < 256; i++)
	{
		tree[i] = new HuffNode;
		tree[i]->f = F.getFrequency(i);
		tree[i]->data = "";
		tree[i]->data = tree[i]->data + char(i);
		tree[i]->left = tree[i]->right = NULL;
	}

	for (; tree.size() > 1;)
	{
		findMinPosition(tree);
		HuffNode* tNode1 = tree[tree.size() - 1];
		tree.erase(tree.begin() + tree.size() - 1);
		findMinPosition(tree);
		HuffNode* tNode2 = tree[tree.size() - 1];
		tree.erase(tree.begin() + tree.size() - 1);

		HuffNode* tNode = new HuffNode;

		tNode->f = tNode1->f + tNode2->f;
		tNode->data = tNode1->data + tNode2->data;

		if (tNode1->f > tNode2->f)
		{
			tNode->left = tNode2;
			tNode->right = tNode1;
		}
		else
		{
			tNode->left = tNode1;
			tNode->right = tNode2;
		}

		tree.push_back(tNode);
	}

	_root = tree[0];
}

vector<vector<char>> HuffmanTree
	::detectCode()
{
	vector<vector<char>> code;
	for (int i = 0; i < 256; i++)
	{
		HuffNode* i_root = _root;
		vector<char> tcode; 
		char c = (char)i;

		for (; i_root->left != NULL;)
		{
			if (find(c, i_root->right->data))
			{
				i_root = i_root->right;
				tcode.push_back(1);
			}
			else
			{
				i_root = i_root->left;
				tcode.push_back(0);
			}
		}

		code.push_back(tcode);
	}

	return code;
}

bool HuffmanTree
	::find(char character, string &s)
{
	int n = s.length();

	if (n == 0)
	{
		return false;
	}

	for (int i = 0; i < n; i++)
	{
		if (character == s[i])
		{
			return true;
		}
	}

	return false;
}

HuffNode* HuffmanTree
	::getRoot()
{
	return _root;
}

string Huffman
	::encode(string &data)
{
	if (data.empty())
	{
		return "";
	}

	unchar table[8] = {128, 64, 32, 16, 8, 4, 2, 1};

	Frequency F;
	F.setFrequency(data);

	HuffmanTree tree;
	tree.buildTree(F);
	vector<vector<char>> code = tree.detectCode();

	ostringstream oss(ios::out | ios::binary);
	
	int longsize = sizeof(unlong);

	for (int i = 0; i < 256; i++)
	{
		unlong f = F.getFrequency(i);
		oss.write((char*)&f, longsize);
	}

	unlong leng = data.length();

	oss.write((char*)&leng, longsize);
	unchar bits = 0;
	int charsize = sizeof(char);

	int numberbit = 0;
	for (unlong i = 0; i < leng; i++)
	{
		unchar c = data[i];

		for (int j = 0; j < code[c].size(); j++)
		{
			if (code[c][j] == 0)
			{
				numberbit++;
			}
			else
			{
				bits |= table[numberbit++];
			}

			if (numberbit == 8)
			{
				oss.write((char*)&bits, charsize);
				bits = 0;
				numberbit = 0;
			}
		}
	}

	if (numberbit != 0)
	{
		oss.write((char*)&bits, charsize);
	}

	return oss.str();
}

string Huffman
	::decode(string &data)
{
	if (data.length() < MIN_FILESIZE)
	{
		return "";
	}

	istringstream iss(data, ios::in | ios::binary);

	unchar table[8] = {128, 64, 32, 16, 8, 4, 2, 1};

	int longsize = sizeof(unlong);
	int charsize = sizeof(char);

	vector<unlong> F;
	F.resize(256);

	for (int i = 0; i < 256; i++)
	{
		iss.read((char*)&F[i], longsize);
	}

	Frequency f;
	f.setFrequency(F);

	HuffmanTree tree;
	tree.buildTree(f);

	HuffNode* tNode = tree.getRoot();
	HuffNode* t = tNode;

	unlong leng = 0;

	iss.read((char*)&leng, longsize);

	unchar bits = 0;
	string rs;
	rs.resize(leng);

	for (unlong i = 0;;)
	{
		iss.read((char*)&bits, charsize);

		for (int numberbit = 0; numberbit < 8; numberbit++)
		{
			if ((bits & table[numberbit]) == 0)
			{
				t = t->left;
			}
			else
			{
				t = t->right;
			}

			if (t->data.length() == 1)
			{
				rs[i] = t->data[0];
				t = tNode;
				i++;
				if (i >= leng)
				{
					return rs;
				}
			}
		}
	}

	return rs;
}