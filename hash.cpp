#include "hash.hpp"

#define M 100
#define C 100
#define CHILDS_NUM 10

using namespace std;

void HashTable::print_hash(){
	cout<<m<<endl;
	for(int i=0;i<m;i++){
		cout<<"Bucket: "<<i<<" ";
		//Bucket  *bu = b[i];
		for(int j=0;j<b[i]->used;j++){
			cout<<j<<": "<<*(b[i]->nodes[j].word)<<"|";
		}
		cout<<endl;

	}
}

//hashTable

HashTable::HashTable(){
	int j=0, k=0;
	mOriginal = M;
	m = M;
	c = C;
	p = 0;
	round = 0;
	overflow = 0;
	b = (Bucket**)malloc(M * sizeof(Bucket*));
	for(j=0; j<M; j++){
		//b[j] = new Bucket();
		b[j] = (Bucket*)malloc(sizeof(Bucket));
		//Bucket* b = new Bucket();
		b[j]->c = C;
		b[j]->used = 0;
		b[j]->cOriginal = C;
		b[j]->nodes = (Trie_node*) malloc(C * sizeof(Trie_node));
		for(int k=0; k<C; k++){
			b[j]->nodes[k].child_num = 0;
			b[j]->nodes[k].capacity = CHILDS_NUM;
			b[j]->nodes[k].children = NULL; //UNINITIALIZED
			b[j]->nodes[k].word = new string();
			*b[j]->nodes[k].word = "";
		}
	}
}



int HashTable::h(string* word){
	int hsum=0;
	for(int i=0; i<(*word).length(); i++)//calculate the character sum to be hashed
		hsum += (int)(*word)[i];
	return hsum % (powerOf2(round) *mOriginal);
}


int HashTable::hN(string* word){
	int hsum=0;
    	for(int j=0; j<(*word).length(); j++)//calculate the character sum to be hashed
		hsum += (int)(*word)[j];
	return hsum % ( powerOf2(round + 1) *mOriginal );
}


int HashTable::powerOf2(int i){ //computes 2^i
	int j, x;
	x=1;
	for(j=0; j<i; j++)
		x*=2;
	return x;
}

//this function will hash the sum of the letters of the node's word

Trie_node* HashTable::linearHashing(Trie_node* node, char is_final){
    //find which bucket it goes in
    Trie_node* ret;
    int hashB = h(node->word);
    //cout<<"w: "<<*node->word<<endl;
    if( hashB >= p ){//choose h(k) hash bucket
	//cout<<" goes to "<<hashB<<" bucket"<<endl;
	//cout<<"w2 "<<*node->word<<" "<<hashB<<" "<<(*ht)->m<<" "<<(*ht)->p<<endl;
	//if((*ht)->b[hashB]==NULL) cout<<"einai null"<<endl;
        ret = b[hashB]->putInBucket(node, is_final,&overflow,0);
	//while(ret==NULL){
	//cout<<"overflow "<<(*ht)->b[hashB]->overflowf<<endl;
	while(overflow>0){ //an overflow occured
		split();
		ret = b[hashB]->putInBucket(node, is_final,&overflow,0);
	}
	return ret;
    }
    else{ //choose hN(k) bucket
	hashB = hN(node->word);
	//cout<<" goes to "<<hashB<<" bucket"<<endl;
	//cout<<"w3 "<<*node->word<<" "<<hashB<<" "<<(*ht)->m<<" "<<(*ht)->p<<endl;
	//if((*ht)->b[hashB]==NULL) (*ht)->b[hashB]->VcreateBucket((*ht)->b, hashB);
	ret =  b[hashB]->putInBucket(node, is_final,&overflow,0);
	while(overflow>0){
		split();
		ret = b[hashB]->putInBucket(node, is_final,&overflow,0);
	}
	return ret;
    }
}


void HashTable::split(){
	int j=0;
	//cout << "SPLIT " <<p<< endl;
	overflow--;
	Bucket* toSplit = b[p]; //the primary page of bucket to be split
	expand(m, m + 1);
	m = m+1;
	transferData(b[p], b[m - 1]); //rehash data between split and new bucket
	p=p+1; //p is incremented
	if( p == (powerOf2(round)* mOriginal)  ){ //advance round if needed
		p = 0;
		round=round+1;
	}
}


void HashTable::expand(int oldsize, int newsize){
	int index;
	//cout<<"Expand: "<<oldsize<<" "<<newsize<<endl;
	//Bucket** temp;
	b=(Bucket**)realloc(b, newsize * sizeof(Bucket*));
	if(b == NULL){
		//free(temp);
		exit(0);
	}
	else{
		//b=temp;
		for(index=oldsize; index < newsize; index++){
			b[index]->VcreateBucket(b, index); //create-initialize the new bucket
		}
	}
	//return b;
}



void HashTable::transferData(Bucket* sp, Bucket* np){
	
	int used = sp->used;
	Trie_node* b_nodes =  (Trie_node*) malloc(used*sizeof(Trie_node));
	memmove(b_nodes,sp->nodes,used*sizeof(Trie_node));
	sp->used=0;
	int hash;
	for(int i=0;i<used;i++){
		hash = hN(b_nodes[i].word);
		if(hash == p){
			sp->putInBucket(&b_nodes[i], b_nodes[i].is_final,&overflow,1);
		}
		else{
			np->putInBucket(&b_nodes[i], b_nodes[i].is_final,&overflow,1);
		}
	}
	/*Bucket** ap;
	ap=sp;
	int Sparser=0, Aparser=0, Hparser=0, h, sused, aused, aflag; //Sparser for (*sp)->entries, Nparser for (*np)->entries and Aparser is auxiliary
	Trie_node* n = new Trie_node();
	sused=(*sp)->used;
	while(Sparser < sused){
		h=hN((*sp)->nodes[Sparser].word); //remap elements between split and new bucket
		//cout<<*((*sp)->nodes[Sparser].word)<<" goes to "<<h<<" bucket"<<endl;
		if(h == p){ //it hashes to the bucket being split
			
   	             	for(Hparser=0; Hparser < Sparser; Hparser++){
                              	if(*(*sp)->nodes[Hparser].word == ""){ //vacant entry found
					(*sp)->makeEntry2(&((*sp)->nodes[Sparser]), Hparser);
					(*sp)->deleteEntry(sp, Sparser);
                                        break;
                                 }
			}
		}
		else{ //it hashes to the new bucket
			n = (*np)->putInBucket(&(*sp)->nodes[Sparser], (*sp)->nodes[Sparser].is_final,&overflow);
			((*sp)->used)--;
			(*sp)->deleteEntry(sp, Sparser);
		}
		Sparser++;
	}*/
}

void HashTable::destroyHashTable(HashTable* ht){
	for(int i=0; i<m; i++){
		Bucket* bt = ht->b[i];
		bt->destroyBucket(bt);
	}
	free(ht->b);
}

void Bucket::destroyBucket(Bucket* bt){
	for(int i=0; i<bt->c; i++){
		bt->nodes[i].destroyNode(nodes[i]);
	}
	free(bt->nodes);
}

//bucket

Bucket::Bucket(){
	this->c = C;
	this->used = 0;
	this->nodes = (Trie_node*) malloc(C * sizeof(Trie_node));
	for(int j=0; j<c; j++){
		this->nodes[j].child_num = 0;
		this->nodes[j].capacity = CHILDS_NUM;
		this->nodes[j].children = NULL; //UNINITIALIZED
		this->nodes[j].word = new string();
		*this->nodes[j].word = "";
	}
}

Bucket* Bucket::createBucket(Bucket** bt){
	*bt = (Bucket*)malloc(sizeof(Bucket)); 	
	(*bt)->c = C;
	(*bt)->cOriginal = C;
	(*bt)->used = 0;
	(*bt)->nodes = (Trie_node*) malloc(C * sizeof(Trie_node));
	for(int j=0; j<c; j++){
		(*bt)->nodes[j].child_num = 0;
		(*bt)->nodes[j].capacity = CHILDS_NUM;
		(*bt)->nodes[j].children = NULL; //UNINITIALIZED
		(*bt)->nodes[j].word = new string();
		*(*bt)->nodes[j].word = "";
	}
	return *bt;
}

void Bucket::VcreateBucket(Bucket** bt, int index){
	bt[index] = (Bucket*)malloc(sizeof(Bucket));
	bt[index]->c = C;
	bt[index]->cOriginal = C;
	bt[index]->used = 0;
	bt[index]->nodes = (Trie_node*) malloc(C * sizeof(Trie_node));
	for(int j=0; j<C; j++){
		bt[index]->nodes[j].child_num = 0;
		bt[index]->nodes[j].capacity = CHILDS_NUM;
		bt[index]->nodes[j].children = NULL;
		bt[index]->nodes[j].word = new string();
		*bt[index]->nodes[j].word = "";
	}
}

Trie_node* Bucket::putInBucket(Trie_node* node, char is_final, int *over,int flag){
	//cout<<"H: "<<*node->word<<" used: "<<(*bt)<<endl;
        int index = BinaryInsertIndex(*node->word, 0, used - 1);
	
	Trie_node* ret;
	if(index<0){
		if(nodes[-1*index-1].is_final=='n' && is_final=='y')
			nodes[-1*index-1].is_final=is_final;
		nodes[-1*index-1].added = 0;
		return &nodes[-1*index-1];
	}
	else{ //only try putting the node in bucket if it doesn't already exist
		index = index -1;
		if(used >= c){ //the bucket is full, an overflow occured
			//cout<<"OVERFLOW"<<endl;
			//cout<<"index "<<index<<endl;
			this->overflow();
			*over = *over +1;
		}
		
		if(index < used){ //needs to be added between node* entries
			int offset = used - index;
			//memmove(&children[index+1],&children[index],offset*sizeof(Trie_node));
			memmove(&nodes[index+1], &nodes[index], offset*sizeof(Trie_node));
			if(flag==0)
				makeEntry(node, index);
			else
				memmove(&nodes[index],node,sizeof(Trie_node));
			used++;
		}
		else{ //needs to be added at the end
			if(flag==0)
				makeEntry(node, index);
			else
				memmove(&nodes[index],node,sizeof(Trie_node));
			used++;
		}
		nodes[index].added = 1;
		return &nodes[index];
    	}
}

void Bucket::makeEntry(Trie_node* node, int index){
	nodes[index].word = new string();
	*nodes[index].word = *node->word;
	nodes[index].children = NULL;
	nodes[index].child_num = 0;
	nodes[index].capacity = CHILDS_NUM;
	nodes[index].is_final = node->is_final;
}

void Bucket::makeEntry2(Trie_node* node, int index){
	nodes[index].word = new string();
	*nodes[index].word = *node->word;
	nodes[index].children = node->children;
	nodes[index].child_num = node->child_num;
	nodes[index].capacity = CHILDS_NUM;
	nodes[index].is_final = node->is_final;
}


void Bucket::deleteEntry(Bucket** bt, int index){
	(*bt)->nodes[index].child_num=0;
	(*bt)->nodes[index].children=NULL;
	*(*bt)->nodes[index].word="";
}


void Bucket::overflow(){ //allocate more space in the same bucket
	int oldsize = c;
	int index;
	//cout<<"old size: "<<cOriginal<<endl;
	c = c + cOriginal;
	//cout<<"new size: "<<c<<endl;
	nodes = (Trie_node*)realloc(nodes, c * sizeof(Trie_node));
	if(nodes == NULL){
		//free(temp);
		exit(0);
	}
	else{
		//(*bt)->nodes=temp;
		for(index=oldsize; index < c; index++){
			nodes[index].child_num = 0;
			nodes[index].capacity = CHILDS_NUM;
			nodes[index].children = NULL;
			nodes[index].word = new string();
			*nodes[index].word = "";
		}
	}
	//return (*bt)->nodes;
}

/*Bucket** HashTable::expand(Bucket** b, HashTable* ht, int oldsize, int newsize){
	int index;
	//cout<<"Expand: "<<oldsize<<" "<<newsize<<endl;
	Bucket** temp;
	temp=(Bucket**)realloc(b, newsize * sizeof(Bucket*));
	if(temp == NULL){
		free(temp);
		exit(0);
	}
	else{
		b=temp;
		for(index=oldsize; index < newsize; index++){
			b[index]->VcreateBucket(b, index); //create-initialize the new bucket
		}
	}
	return b;
}*/

int Bucket::empty(Bucket* bt){
	if(bt->used == 0)
		return 1;
	else
		return 0;
}


int Bucket::BinaryInsertIndex(string word, int low, int high){
	if (high <= low){
		if(used==0 || nodes[low].word==NULL){
			nodes[low].word = new string();
			//cout<<word<<endl;
			return low+1;
		}
		if(!nodes[low].word->empty()){
			if(word.compare(*nodes[low].word) == 0){
				return -1*(low+1);
			}
			else{
        			return (word.compare(*nodes[low].word) > 0)?  (low + 2): (low+1);
			}
		}
		else{
			return low+1;
		}
	}

    	int mid = (low + high)/2;

    	if(word.compare(*nodes[mid].word) == 0)
        	return -1*(mid+1);

    	if(word.compare(*nodes[mid].word) > 0)
        	return BinaryInsertIndex(word,mid+1, high);
    	return BinaryInsertIndex(word, low, mid-1);


}



//search for word in a hash bucket
Trie_node* Bucket::lookupBucket(string word){
	int index;
	Trie_node* node;
	index = BinarySearch(word, 0,used - 1);
	//cout<<"found index: "<<index;
	if( index >=0 ) //word found in this bucket
		return &nodes[index];
	return NULL; //not found
}


int Bucket::BinarySearch(string word,int low,int high){

	if (low <= high){
        	int mid = low + (high - low)/2;

        	if (word.compare(*nodes[mid].word) == 0)  return mid;
        	if (word.compare(*nodes[mid].word) > 0) return BinarySearch(word, mid+1 ,high);
        	return BinarySearch(word, low, mid-1);
   	}

   	return -1;
}


int Bucket::removeFromBucket(string* ngram, int words_num, int i){
	int index, ret;
	Trie_node * child;
	//cout<<"diag "<<ngram[0]<<" "<<i<<endl;
	//if(i<words_num-1){
		index = BinaryInsertIndex(ngram[i],0, used-1);
		//cout<<"dia "<<index<<" "<<ngram[0]<<endl;
		if(index < 0){ //the word exists in the bucket
			index = -1*index-1;
			child = &nodes[index];
			//cout<<"dia "<<*child->word<<" "<<child->child_num<<endl;
			ret=0;
			if(words_num>1)
				child->Delete(child,ngram,words_num,i+1);
			if(words_num==1 && child->is_final=='y'){
				child->is_final='n';
				//cout<<"mphke "<<*child->word<<endl;
			}
			if(child->child_num==0 && child->is_final=='n'){
				//cout<<"mphke "<<*child->word<<endl;
				if(used-1>0){
					int offset = used-index;
					memmove(&nodes[index],&nodes[index+1],offset*sizeof(Trie_node));
					nodes[used-1].child_num=0;
					nodes[used-1].children=NULL;
					//cout<<"mphke "<<*child->word<<endl;
					nodes[used-1].word=NULL;
					//cout<<"mphke "<<*child->word<<endl;
				}
				used--;
				return 1;
			}
			else {
				return 0;
			}	
		}
		else
			return -1;
	
}



