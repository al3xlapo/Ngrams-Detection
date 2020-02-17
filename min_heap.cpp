#include <stdlib.h>
#include "min_heap.hpp"

#define LCHILD(x) 2 * x + 1
#define RCHILD(x) 2 * x + 2
#define PARENT(x) (x - 1) / 2

Min_heap * Create_min_heap(){

	Min_heap * h = (Min_heap *) malloc(sizeof(Min_heap));
	h->hashtable = new map<string,int>();
	return h;
}

void initializeMin_heap(Min_heap * h, int size){

	h->heap_array = (Node *) malloc(size*sizeof(Node));
	for(int i=0;i<size;i++){
		h->heap_array[i].ngram = new string();
	}
	h->size = 0;
	h->capacity = size;
}

void resetHash(Min_heap * h){

	h->hashtable->clear();
}

void addNgram(Min_heap * h, string ngram){

	map<string,int>::iterator it = h->hashtable->find(ngram);
	if(it==h->hashtable->end()){
		h->hashtable->insert(pair<string,int>(ngram,1));
	}
	else{
		it->second++;
	}
}

void swap(Node *n1, Node *n2) {
    Node temp = *n1 ;
    *n1 = *n2 ;
    *n2 = temp ;
}

void heapify(Min_heap * h, int i) {
    int smallest; 
    if(LCHILD(i) < h->size && h->heap_array[LCHILD(i)].occ ==h->heap_array[i].occ){
	if(h->heap_array[LCHILD(i)].ngram->compare(*h->heap_array[i].ngram)>0){
		smallest = LCHILD(i);
	}
	else{
		smallest = i;
	}
    }
    else{
	smallest =(LCHILD(i) < h->size && h->heap_array[LCHILD(i)].occ < h->heap_array[i].occ) ? LCHILD(i) : i ;
    }
    if(RCHILD(i) < h->size && h->heap_array[RCHILD(i)].occ ==h->heap_array[smallest].occ){
	if(h->heap_array[RCHILD(i)].ngram->compare(*h->heap_array[smallest].ngram)>0){
		smallest = RCHILD(i);
	}
    }
    else{
	if(RCHILD(i) < h->size && h->heap_array[RCHILD(i)].occ < h->heap_array[smallest].occ) {
        	smallest = RCHILD(i) ;
    	}
    }
    if(smallest != i) {
        swap(&(h->heap_array[i]), &(h->heap_array[smallest])) ;
        heapify(h, smallest) ;
    }
}

void deleteNode(Min_heap * h) {

    if(h->size!=0) {
	h->size--;
        h->heap_array[0].occ = h->heap_array[(h->size)].occ;
	*h->heap_array[0].ngram = *h->heap_array[(h->size)].ngram;
        heapify(h, 0) ;
    } 
}

string TakeRoot(Min_heap * h){

	string ngram;
	 if(h->size!=0) {
		ngram = *h->heap_array[0].ngram;
		h->size--;
        	h->heap_array[0].occ = h->heap_array[(h->size)].occ;
		*h->heap_array[0].ngram = *h->heap_array[(h->size)].ngram;
        	heapify(h, 0) ;
		return ngram;
	}
	else
		return "";
}

void insertNode(Min_heap * h,string ngram,int occ){

	int i;
	if(h->size < h->capacity){
		i = h->size;
		while(i!=0 && occ < h->heap_array[PARENT(i)].occ) {
			*h->heap_array[i].ngram = *h->heap_array[PARENT(i)].ngram;
			h->heap_array[i].occ = h->heap_array[PARENT(i)].occ;
			i = PARENT(i);
		}
		while(i!=0 && occ==h->heap_array[PARENT(i)].occ){
			if(ngram.compare(*h->heap_array[PARENT(i)].ngram)>0){
				h->heap_array[i].occ = h->heap_array[PARENT(i)].occ;
				*h->heap_array[i].ngram = *h->heap_array[PARENT(i)].ngram;
			}
			i = PARENT(i);
		}
		h->heap_array[i].occ = occ;
		*h->heap_array[i].ngram = ngram;
		h->size++;
	}
	else{
		if(h->heap_array[0].occ < occ ){
			deleteNode(h);
			insertNode(h,ngram,occ);
		}
	}
}

void Print(Min_heap * h){
	
	for(int i=0;i<h->size;i++){
		cout<<" index: "<<i<<" ngram: "<<*h->heap_array[i].ngram<<" occ: "<<h->heap_array[i].occ<<endl;
	}
}


string FindtopK(Min_heap * h){

	string topK;
	for (map<string,int>::iterator it=h->hashtable->begin(); it!=h->hashtable->end(); ++it){
		insertNode(h,it->first,it->second);
	}
	int size = h->size;
	for(int i=0;i<size;i++){
		topK = "|"+TakeRoot(h)+topK;
	}
	return topK;
}


void resetMin_heap(Min_heap * h){

	for(int i=0;i<h->size;i++){
		delete h->heap_array[i].ngram;
	}
	free(h->heap_array);
	//h->size=0;
	//h->capacity = size;
	h->hashtable->clear();
}

void destroyMin_heap(Min_heap * h){

	for(int i=0;i<h->size;i++){
		delete h->heap_array[i].ngram;
	}
	free(h->heap_array);
	delete h->hashtable;
	free(h);
}

