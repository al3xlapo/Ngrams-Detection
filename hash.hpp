#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "trieNode.hpp"

using namespace std;

class Bucket{

public:
	Trie_node* nodes; //array of pointers to Trie_node
	int c, used, cOriginal; //c: how many node pointers a bucket can take(size of nodes array), used: how many are occcupied
	int overflowf; //it turns 1 to let the linearHashing know there's been an overflow

public:
	Bucket();
	~Bucket();
	void VcreateBucket(Bucket** bt, int index);
	Bucket* createBucket(Bucket** bt);
	Trie_node* putInBucket(Trie_node* node, char is_final,int *over,int flag);
	void makeEntry(Trie_node* node, int index);
	void makeEntry2(Trie_node* node, int index);
	int removeFromBucket(string* ngram, int words_num, int i);
	int BinaryInsertIndex(string word, int low, int high);
	Trie_node* lookupBucket(string word);
	int BinarySearch(string word,int low,int high);
	int deleteBucket();
	void deleteEntry(Bucket** bt, int index);
	void printBucket();
	void overflow();
	void destroyBucket(Bucket* bt);
	int empty(Bucket* bt);
};

class HashTable{

public:
	int mOriginal, m, c; //mOriginal is the number of primary buckets and will be stable, m will increase with the hashtable and c is the number of nodes in each bucket
	int round, p; // i is the current splitting round, p is the bucket to be split
	Bucket** b; //array of pointers to buckets
	int overflow;
	
public:
	void print_hash();
	HashTable();
	~HashTable();
	void createLinearHash(HashTable**);
	void destroyHashTable(HashTable* ht);
	int h(string* word); //hash functions needed at any round
	int hN(string* word);
	int powerOf2(int i);
    	Trie_node* linearHashing(Trie_node* node, char is_final);
	void expand(int oldsize, int newsize);
	void split();
	void transferData(Bucket* sp, Bucket* np);
};


