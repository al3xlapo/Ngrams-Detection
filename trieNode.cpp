#include "trieNode.hpp"

#define CHILDS_NUM 10

using namespace std;

Trie_node::Trie_node(){

	word = new string();
	*word = "";
	children = NULL;
	child_num = 0;
	capacity = CHILDS_NUM;
}

Trie_node::~Trie_node(){
	//free(children);
}

void Trie_node::destroyNode(Trie_node node){
	delete word;
	for(int i=0; i<child_num; i++){
		children[i].destroyNode(children[i]);
	}
	free(children);
}

Trie_node * Trie_node::Insert(string word,char is_final){

	int index = BinaryInsertIndex(word, 0, child_num-1);
	if(index<0){
		if(children[-1*index-1].is_final=='n' && is_final=='y')
			children[-1*index-1].is_final=is_final;
		return &children[-1*index-1];
	}
	else{
		index = index -1;
	}
	if(child_num ==0) capacity = CHILDS_NUM;
	if(child_num >= capacity){
		Trie_node * temp = (Trie_node *) realloc(children,2*capacity*sizeof(Trie_node));
		memset(temp+capacity,0,capacity*sizeof(Trie_node));
		children = temp;
		capacity = capacity*2;
	}

	if(index<=child_num-1){
		int offset = child_num - index;
		memmove(&children[index+1],&children[index],offset*sizeof(Trie_node));
		child_num++;
		children[index].word = new string();
		*children[index].word = word;
		children[index].children = NULL;
		children[index].child_num = 0;
		children[index].capacity = 10;
		children[index].is_final = is_final;
	}
	else{
		child_num++;
		children[index].word = new string();
		*children[index].word = word;
		children[index].is_final = is_final;
	}
	return &children[index];


}

int Trie_node::BinaryInsertIndex(string word, int low, int high){

	if (high <= low){
			if(child_num==0 || children[low].word==NULL){
				return low+1;

			}
		if(!children[low].word->empty()){
			if(word.compare(*children[low].word) == 0){
				return -1*(low+1);
			}
			else{
        			return (word.compare(*children[low].word) > 0)?  (low + 2): (low+1);
			}
		}
		else{
			return low+1;
		}
	}
 
    	int mid = (low + high)/2;
 
    	if(word.compare(*children[mid].word) == 0)
        	return -1*(mid+1);
 
    	if(word.compare(*children[mid].word) > 0)
        	return BinaryInsertIndex(word,mid+1, high);
    	return BinaryInsertIndex(word, low, mid-1);

	
}


int Trie_node::Delete(Trie_node * parent, string * ngram, int words_num, int i){

	int return_val;
	int index;
	Trie_node * child;
	if(i<words_num-1){
		index = parent->BinaryInsertIndex(ngram[i],0,parent->child_num-1);
		if(index<0){
			index = -1*index-1;
			child = &(parent->children[index]);
			return_val = Delete(child,ngram,words_num,i+1);
			if(return_val==1){
				if(child->child_num == 0 && child->is_final=='n'){
					if(parent->child_num-1>0){
						free(parent->children[index].children);
						delete parent->children[index].word;
						int offset = parent->child_num-1-index;
						memmove(&parent->children[index],&parent->children[index+1],offset*sizeof(Trie_node));
						parent->children[parent->child_num-1].child_num=0;
						parent->children[parent->child_num-1].children=NULL;
						parent->children[parent->child_num-1].word=NULL;
					}
					else{
						if(parent->children[index].children!=NULL){
							free(parent->children[index].children);
							parent->children[index].children=NULL;
						}
						delete parent->children[index].word;
					}
					parent->child_num--;
					return 1;
				}
				else{
					return 0;
				}
			}
			else{
				return return_val;				
			}
			
		}
		else{
			return -1;
		}
	}
	else{
		index = parent->BinaryInsertIndex(ngram[i],0,parent->child_num-1);
		if(index<0){
			index = -1*index-1;
			child = &(parent->children[index]);
			if(child->child_num==0){ //diagrafh komvoy an den exei paidia
				if(parent->child_num-1>0){
					free(parent->children[index].children);
					delete parent->children[index].word;
					int offset = parent->child_num-1-index;
				  memmove(&parent->children[index],&parent->children[index+1],offset*sizeof(Trie_node));
					parent->children[parent->child_num-1].child_num=0;	
					parent->children[parent->child_num-1].children=NULL;
					parent->children[parent->child_num-1].word=NULL;
					
				}
				else{
					if(parent->children[index].children!=NULL){
						free(parent->children[index].children);
						parent->children[index].children = NULL;
					}
					delete parent->children[index].word;
				}
				parent->child_num--;
				return 1;
			}
			else{
				child->is_final = 'n';
				return 0;
			}
			
		}
		else{
			return -1;
		}
	}
}

int Trie_node::Static_BinarySearch(string word,int low,int high){

	if (low <= high){
        	int mid = low + (high - low)/2;
		if(children[mid].words_num==0){
        		if (word.compare(*children[mid].word) == 0)  return mid;
        		if (word.compare(*children[mid].word) > 0) return Static_BinarySearch(word,mid+1,high);
        		return Static_BinarySearch(word,low,mid-1);
		}
		else{
			int len;
			if(children[mid].wordsInfo[0]<0)
				len = -1*children[mid].wordsInfo[0];
			else
				len = children[mid].wordsInfo[0];
			string temp = children[mid].word->substr(0,len);
			if (word.compare(temp) == 0)  return mid;
        		if (word.compare(temp) > 0) return Static_BinarySearch(word,mid+1,high);
        		return Static_BinarySearch(word,low,mid-1);
		}
   	}
 
   	return -1;
}


void Trie_node::PrintChilds(){
	cout << " Number of childs: " << child_num<< endl;
	for(int i=0;i<child_num;i++){
		cout<<*children[i].word<<endl;
	}
}


int Trie_node::BinarySearch(string word,int low,int high){

	if (low <= high){
        	int mid = low + (high - low)/2;

        	if (word.compare(*children[mid].word) == 0)  return mid;
        	if (word.compare(*children[mid].word) > 0) return BinarySearch(word,mid+1,high);
        	return BinarySearch(word,low,mid-1);
   	}

   	return -1;
}



