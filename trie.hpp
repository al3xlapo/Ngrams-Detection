#include <iostream>
#include <string.h>

#include "hash.hpp"
#include "bloom.hpp"
#include "min_heap.hpp"

using namespace std;



class TrieRoot{ //the root of trie has its own class to contain the hashTable

public:
	HashTable* ht; //linear hashTable, only the root has it
	string word;
	int child_num;

public:
	TrieRoot();
	~TrieRoot();
	int BinaryInsertIndex(string word, int low, int high);
	Trie_node * Insert(string word,char is_final);
	int Delete(Trie_node * node,string * ngram, int words_num, int i);
	void PrintChilds();
	int BinarySearch(string word,int low,int high);
	//HashTable functions
	void createLinearHash();
	Trie_node* insertTrieNode(Trie_node* node, char is_final);
	Trie_node* lookupTrieNode(string* word);
	int deleteTrieNode(string * ngram, int words_num, int i);
	void destroyLinearHash(HashTable* ht);
};


class Trie {

public:	TrieRoot* root;
	void InsertNgram(string * ngram, int words_num);
	void DeleteNgram(string * ngram, int words_num);
	//void SearchNgram(string * text,int words_num,string *out);
	void SearchNgram(string * text,int words_num,string *out,Bloom * filter, Min_heap * h);
	void Static_SearchNgram(string * text,int words_num,string *out,Bloom * filter, Min_heap * h);
	void PrintTrie(Trie_node * a);
	void Print();
	void DeleteTrie(Trie_node * a);
	void Compress();
	void Compress2(Trie_node * temp,char is_root);
	void Join2Nodes(Trie_node * a,Trie_node * b);

public:
	Trie();
	~Trie();
};
