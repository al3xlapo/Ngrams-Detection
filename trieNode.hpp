#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <stdlib.h>
#include <cmath>

using namespace std;


class Trie_node {


public:	string * word;
	Trie_node * children;
	signed short * wordsInfo;
	unsigned int words_num;
	int child_num;
	int capacity;
	char is_final;
	int added; //auxiliary variable that equals 0 when an insert actually inserts nothing in a bucket of the hashtable(the node already exists)

public:
	Trie_node();
	~Trie_node();
	int BinaryInsertIndex(string word, int low, int high);
	Trie_node * Insert(string word,char is_final);
	int Delete(Trie_node * node,string * ngram, int words_num, int i);
	void PrintChilds();
	int BinarySearch(string word,int low,int high);
	void destroyNode(Trie_node node);
	int Static_BinarySearch(string word,int low,int high);
	//int Search(string word);
	//int SearchIndex(string word,int first,int last);

};

