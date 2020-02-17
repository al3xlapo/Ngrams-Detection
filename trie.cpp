#include <iostream>
#include <string>
#include <stdlib.h>

#include "trie.hpp"

#define CHILDS_NUM 10

using namespace std;


Trie::Trie(){

	root = new TrieRoot();
}

Trie::~Trie(){

	//delete root;
}


TrieRoot::TrieRoot(){
	ht = NULL;
	word = "root";
	child_num = 0;

}


TrieRoot::~TrieRoot(){


}


void Trie::InsertNgram(string * ngram, int words_num){

	char is_final = 'n';
	int i = 0;
	//Trie_node* node = new Trie_node();
	Trie_node* node = new Trie_node();
	*node->word = ngram[0];
	if(i==words_num-1)
		is_final = 'y';
	node->is_final = is_final;
	Trie_node* current_layer = this->root->insertTrieNode(node, is_final);
	if(current_layer==NULL) return;
    	//hash function
	for(i=1;i<words_num;i++){ //omit first word, it will go in root hashtable
		if(current_layer->children == NULL){
			current_layer->children = (Trie_node *) malloc(CHILDS_NUM*sizeof(Trie_node));
			memset(current_layer->children,0,CHILDS_NUM*sizeof(Trie_node));
		}
		if(i==words_num-1)
			is_final = 'y';
		current_layer = current_layer->Insert(ngram[i],is_final);
	}
}


void Trie::DeleteNgram(string * ngram, int words_num){

	this->root->deleteTrieNode(ngram, words_num, 0);
	//root->Delete(root,ngram,words_num,0);
}

void Trie::SearchNgram(string * text,int words_num,string * output,Bloom * filter,Min_heap * h){

	int index;
	Trie_node * temp = this->root->lookupTrieNode(&text[0]);
	string match="";
	if(temp == NULL){ //initial word not found in hashtable
		//cout<<text[0]<<" den vre8hke "<<endl;
		return;
	}
	else{
		//cout<<" vre8hke "<<*temp->word<<endl;
	}
	if(match.empty())	
		match = *temp->word;
	else
		match += " "+*temp->word;
	if(temp->is_final == 'y'){
		if(output->empty()){
				*output = "|"+match+"|";
				add_word(match.c_str(),match.length(),filter);
				addNgram(h,match);
			}
			else{
				if(!check_word(match.c_str(),match.length(),filter)){
					*output += match+"|";
					add_word(match.c_str(),match.length(),filter);
					addNgram(h,match);
				}
				/*if(output->find("|"+match+"|")==string::npos){
					*output += match+"|";
				}*/
			}
	}		
	for(int i=1;i<words_num;i++){
		index = temp->BinarySearch(text[i],0,temp->child_num-1);
		//cout<<"index: "<<index<<endl;
		if(index==-1)
			break;
		if(match.empty())	
			match = *temp->children[index].word;
		else
			match += " "+*temp->children[index].word;
		if(temp->children[index].is_final=='y'){
			if(output->empty()){
				*output = "|"+match+"|";
				add_word(match.c_str(),match.length(),filter);
				addNgram(h,match);
			}
			else{
				if(!check_word(match.c_str(),match.length(),filter)){
					*output += match+"|";
					add_word(match.c_str(),match.length(),filter);
					addNgram(h,match);
				}
				/*if(output->find("|"+match+"|")==string::npos){
					*output += match+"|";
				}*/
			}
		}
		temp = &temp->children[index];
	}

}


void Trie::Static_SearchNgram(string * text,int words_num,string *output,Bloom * filter,Min_heap * h){

	//Trie_node * temp = root;
	Trie_node * temp = this->root->lookupTrieNode(&text[0]);
	if(temp == NULL){ //initial word not found in hashtable
		return;
	}
	int index;
	string match="";
	match = *temp->word;
	if(temp->is_final == 'y'){
		if(output->empty()){
				*output = "|"+match+"|";
				add_word(match.c_str(),match.length(),filter);
				addNgram(h,match);
			}
			else{
				if(!check_word(match.c_str(),match.length(),filter)){
					*output += match+"|";
					add_word(match.c_str(),match.length(),filter);
					addNgram(h,match);
				}
			}
	}
	int i=1;
	while(i<words_num){
		index = temp->Static_BinarySearch(text[i],0,temp->child_num-1);
		if(index==-1)
			break;
		if(temp->children[index].words_num==0){
			if(match.empty())
				match = *temp->children[index].word;
			else
				match += " "+*temp->children[index].word;
			if(temp->children[index].is_final=='y'){
				if(output->empty()){
					*output = "|"+match+"|";
					add_word(match.c_str(),match.length(),filter);
					addNgram(h,match);
				}
				else{
					if(!check_word(match.c_str(),match.length(),filter)){
						*output += match+"|";
						add_word(match.c_str(),match.length(),filter);
						addNgram(h,match);
					}
				}
			}			
			i++;
			temp = &temp->children[index];
		}
		else{
			string w;
			char is_final;
			int offset=0;
			short len;
			for(int j=0;j<temp->children[index].words_num;j++){
				
				if(temp->children[index].wordsInfo[j]<0){
					len = -1*temp->children[index].wordsInfo[j];
					is_final = 'n';
				}
				else{
					len = temp->children[index].wordsInfo[j];
					is_final = 'y';
				}
				w = temp->children[index].word->substr(offset,len);
				if(text[i].compare(w)==0){
					if(match.empty())
						match = w;
					else
						match += " "+w;
					if(is_final=='y'){
						if(output->empty()){
							*output = "|"+match+"|";
							add_word(match.c_str(),match.length(),filter);
							addNgram(h,match);
						}
						else{
							if(!check_word(match.c_str(),match.length(),filter)){
								*output += match+"|";
								add_word(match.c_str(),match.length(),filter);
								addNgram(h,match);
							}
						}
					}
				}
				else{
					return;
				}
				offset += len;
				i++;
			}
			temp = &temp->children[index];
		}
	}

}


void Trie::PrintTrie(Trie_node * a){

	int i;
	Trie_node * temp = a;
	if( a->children ==NULL) return;
	cout<< "Node '"<<*temp->word<<"' -> " ;
	for(i=0;i<temp->child_num;i++){
		cout << *temp->children[i].word << " "<<temp->children[i].is_final<<" ";
	}
	cout << endl;
	for(i=0;i<temp->child_num;i++){
		PrintTrie(&temp->children[i]);
	}

}

void Trie::Print(){

	/*cout<<"Trie: "<<endl;
	PrintTrie(root);
	cout<<endl;*/
}



//hash functions

void TrieRoot::createLinearHash(){
	ht = new HashTable();
	
}


Trie_node* TrieRoot::insertTrieNode(Trie_node* node, char is_final){
	int h;
	Trie_node* n = new Trie_node();
	n = ht->linearHashing(node, is_final);
	if(n==NULL) return NULL;
	if(n->added != 0)
		child_num++;
	return n;
}


Trie_node* TrieRoot::lookupTrieNode(string* word){
    //find the bucket the node could be in
    int hashB = ht->h(word);
    if( hashB >= ht->p ){//choose h(k) hash bucket
	//cout<<"hash "<<hashB<<endl;
        return ht->b[hashB]->lookupBucket(*word);
    }
    else{ //choose hN(k) bucket
	//cout<<"hash "<<ht->hN(word)<<endl;
	return ht->b[ht->hN(word)]->lookupBucket(*word);
    }

}


int TrieRoot::deleteTrieNode(string * ngram, int words_num, int i){
	int index, ret;
	//find the bucket the node could be in
    	int hashB = ht->h(&ngram[0]);
    	if( hashB >= ht->p ){//choose h(k) hash bucket
       		ret = ht->b[hashB]->removeFromBucket(ngram, words_num, i);
		if(ret > 0) //deletion made
			child_num--;
		return ret;
	}
	else{ //choose hN(k) bucket
		ret =  ht->b[ht->hN(&ngram[0])]->removeFromBucket(ngram, words_num, i);	
		if(ret > 0) //deletion made
			child_num--;
		return ret;
	}
}




void TrieRoot::destroyLinearHash(HashTable* ht){
	ht->destroyHashTable(ht);
	free(ht);
}


void Trie::Compress(){

	HashTable * h = root->ht;
	Bucket * buck;
	for(int i=0;i<h->m;i++){
		buck  = h->b[i];
		for(int j=0;j<buck->used;j++){
			Compress2(&buck->nodes[j],'y');
		}
	}

}

void Trie::Compress2(Trie_node * temp,char is_root){

	if(temp->child_num>1){
		for(int i=0;i<temp->child_num;i++){
			Compress2(&temp->children[i],'n');
		}
	}
	else if(temp->child_num==1){
		if(is_root=='n'){
			Join2Nodes(temp,&temp->children[0]);
			temp->child_num = temp->children[0].child_num;
			temp->children = temp->children[0].children;
			Compress2(temp,'n');
		}
		else{
			Compress2(&temp->children[0],'n');
		}
	}
	else{
		return;
	}

}


void Trie::Join2Nodes(Trie_node * a, Trie_node * b){

	if(a->wordsInfo==NULL){
		a->wordsInfo = (signed short *) malloc(2*sizeof(signed short));
		a->words_num = 2;
		if(a->is_final=='y'){
			a->wordsInfo[0] = a->word->length();
		}
		else{
			a->wordsInfo[0] = -1*a->word->length();
		}
		if(b->is_final=='y'){
			a->wordsInfo[1] = b->word->length();
		}
		else{
			a->wordsInfo[1] = -1*b->word->length();
		}
		*a->word += *b->word;
	}
	else{
		a->wordsInfo = (signed short *) realloc(a->wordsInfo,(a->words_num+1)*sizeof(signed short));
		a->words_num ++;
		if(b->is_final=='y'){
			a->wordsInfo[a->words_num-1] = b->word->length();
		}
		else{
			a->wordsInfo[a->words_num-1] = -1*b->word->length();
		}
		*a->word += *b->word;
	}
}




