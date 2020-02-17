#include <iostream>
#include <string.h>
#include <map>

using namespace std;

typedef struct {
	string * ngram;
	int occ;
} Node;

typedef struct {
     map<string,int> * hashtable;
     Node * heap_array;
     int size;
     int capacity;

} Min_heap;

Min_heap * Create_min_heap();

void initializeMin_heap(Min_heap * h, int size);

void resetHash(Min_heap * h);

void resetMin_heap(Min_heap * h);

void destroyMin_heap(Min_heap * h);

void addNgram(Min_heap * h, string ngram);

void swap(Node *n1, Node *n2);

void heapify(Min_heap * p, int i);

void deleteNode(Min_heap * h);

string TakeRoot(Min_heap * h);

void insertNode(Min_heap * h,string ngram,int occ);

void Print(Min_heap * h);

string FindtopK(Min_heap * h);
