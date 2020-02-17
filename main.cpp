#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <map>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>


#include "jobScheduler.hpp"

#define M 100
#define C 100
#define CHILDS_NUM 10
#define ThreadNum 10

using namespace std;

int main (int argc, char** argv) {

char *i_file;
char *q_file;
int ar;
while ((ar = getopt(argc, argv, ":i:q:")) != -1) {

	switch(ar) {
		case 'i':
			i_file= optarg;
			break;
		case 'q':
			
			q_file= optarg;
			break;
		case '?':
			fprintf(stderr,"Unrecognized option: -%c\n", optopt);
	}
}

	Min_heap * min_heap = Create_min_heap();
	int query_num=0;
	int max=0;
	string *out;
	string * text_ptr;
	char *adq_job = new char[15000];
	adq_job[0] = 'F';
	int adq_num = 0;
	int current_version = 0;
	int version_A = 0;
	int version_D = 0;
	//memory allocation
	Trie * trie = new Trie();
	int j=0, k=0;
	trie->root->createLinearHash(); 
	string * ngram = new string[150000];
	Bloom * filter = Bloom_create(100000,5);
	int capacity = 150000;
	int word_count = 0;
	string line,word;
	char is_dynamic ;
	ifstream myfile (i_file); //διαβασμα του αρχικου αρχειου για δημιουργια του δεντρου
	int i=1;
	int line_num=0;
  	if (myfile.is_open())
  	{
		//trie->root->ht->print_hash();
  	 	while ( getline (myfile,line) )
    		{
			//cout<<i<<" word= "<<line<<endl;
      			if(i==1){
				if(line.compare("STATIC")==0){
					is_dynamic = 'n';
					i++;
					continue;
				}
				else if(line.compare("DYNAMIC")==0){
					is_dynamic = 'y';
					i++;
					continue;
				}
				else{
					i++;
				}
			}
      			//cout << line << '\n';
			istringstream is(line); // σπαει τη καθε γραμμη σε λεξεις
			
			
			while(is >> word){
				if(word_count >= capacity){
					//cout<<"diplasiasmos"<<endl;
					string * temp = new string[2*capacity];
					for(int i=0;i<capacity;i++){
						temp[i]=ngram[i];
					}
					capacity = 2*capacity;
					ngram = temp;
					ngram[word_count]=word;
				}
				else{
					ngram[word_count] = word;
				}
				word_count++;
			}
			trie->InsertNgram(ngram,word_count);		
			i++;
			word_count=0;

    		}
    	myfile.close();
  	}

  	else 
		cout << "Unable to open file"; 
	JobScheduler *jobSch = new JobScheduler(ThreadNum, trie, is_dynamic); 
	ifstream myfile2 (q_file); //διαβασμα του αρχικου αρχειου για δημιουργια του δεντρου
  	if (myfile2.is_open()){
		if(is_dynamic=='y'){
			while ( getline (myfile2,line) ){

				line_num++;
				istringstream is(line);
				//cout<<line<<endl;
				is >> word;
				if(word.compare("A")==0){
					//cout<<line<<endl;
					i=0;
					while(is >> word){
						ngram[i]=word;
						i++;
					}
					adq_num++;
					adq_job[adq_num] = 'A';
					if (adq_job[adq_num -1 ] == 'A' || adq_job[adq_num -1 ] == 'D')
						version_A = current_version;
					else if (adq_job[adq_num -1 ] == 'Q')
					{
						current_version++;
						version_A = current_version;
					}
					else if (adq_job[adq_num -1 ] == 'F')
						current_version = 0;
					trie->InsertNgram(ngram,i);	

				}
				else if(word.compare("D")==0){
					//cout<<line<<endl;
					i=0;
					while(is >> word){
						ngram[i]=word;
						i++;
					}
					adq_num++;
					adq_job[adq_num] = 'D';
					if (adq_job[adq_num -1 ] == 'A' || adq_job[adq_num -1 ] == 'D')
						version_D = current_version;
					else if (adq_job[adq_num -1 ] == 'Q')
					{
						current_version++;
						version_D = current_version;
					}
					else if (adq_job[adq_num -1 ] == 'F')
						current_version = 0;
					trie->DeleteNgram(ngram,i);
				}
				else if(word.compare("F")==0){
					if(query_num>1){
						if(is >> word){
							int k = atoi(word.c_str());
							initializeMin_heap(min_heap,k);
				     			string top_k = FindtopK(min_heap);
							cout<<"Top: "+top_k.substr(1,top_k.size()-1)<<endl;
							resetMin_heap(min_heap);
						}
						else{
							resetHash(min_heap);
						}
						query_num=0;
						adq_num = 0;
						adq_job[0] = 'F';
					}
				}
				else{
					out=new string();
					i=0;
					adq_num++;
					adq_job[adq_num] = 'Q';
					if(word.compare("Q")==0){
						query_num++;
						is >> word;
						ngram[i]=word;
						i++;
					}
					while(is >> word){
						ngram[i]=word;
						i++;
					}
					for(int j=0;j<i;j++){
						//if(line_num==36)
						trie->SearchNgram(&ngram[j],i-j,out,filter,min_heap);
					}
					if(out->empty()){
						//if(line_num==36)
						cout<<-1<<endl;
					}
					else{
						*out = out->substr(1, out->size()-2);
						cout<<*out<<endl;
					}
					reset(filter);
					delete out;

				}
			
			}
		}
		else{ //static
			trie->Compress();
			while ( getline (myfile2,line) ){
				
				line_num++;
				istringstream is(line);

				is >> word;
				if(word.compare("F")==0){
					query_num++;
					string* results = new string[query_num];
					jobSch->results = results;
					is >> word;

					int k = atoi(word.c_str());
					Job* j = new Job(query_num , 0, 0, 0, 'F', NULL,0,k, trie, min_heap);
					jobSch->executeAllJobs();
					jobSch->waitTasks(0);
					
					
					//perform topk
					if ( j->k > 0)
					{
						initializeMin_heap(j->min_heap, j->k);
						string top_k = FindtopK(j->min_heap);
						jobSch->results[j->number - 1] = "Top: "+top_k.substr(1,top_k.size()-1)+"\n";
						resetMin_heap(j->min_heap);
					}
					else{
						resetHash(j->min_heap);
					}
					//print results
					for(int l=0; l<query_num; l++) //print stored results("\n" is already stored in string)
						cout<<jobSch->results[l];
					query_num = 0; // οταν βρεις F κανε τα query_num μηδεν
					delete j;
					delete[] results;
				}
				else{
					string * ngram = new string[150000];
					//string* ngram = new string;
					//string wordN;
					i=0;
					if(word.compare("Q")==0){
						query_num++;
						is >> word;
						ngram[i]=word;
						//is >> wordN;
						//wordN += " ";
						//*ngram += word;
						i++;
					}
					while(is >> word){
					//while(is >> wordN){
						//wordN += " ";
						ngram[i]=word;
						//*ngram += wordN;
						i++;
					}
					
					Job* j = new Job(query_num, 0, 0, 0, 'Q', ngram,i, 0, trie, min_heap);
					jobSch->submitJob(j);
					delete[] ngram;
				}
			}
			
		}
		myfile2.close();
	}
	//trie->DeleteTrie(trie->root);
	delete trie;
	delete[] ngram;
	Bloom_destroy(filter);
/*trie->root->ht->print_hash();
cout<<"aa "<<(trie->root->ht->b[121]->nodes[19].child_num)<<endl;
for(int i=0;i<(trie->root->ht->b[121]->nodes[19].child_num);i++){
	cout<<i<<": "<<*(trie->root->ht->b[121]->nodes[19].children[i].word)<<" | ";
}
	string* n = new string[2];
	n[0]= "the";
	n[1]= "work";
	string a;
	trie->SearchNgram(&n[0],2,&a);
	cout<<"w: "<<a<<endl;*/
	//trie->root->destroyLinearHash(trie->root->ht); 
	//free(trie->root);
	//free(trie);
  return 0;
}
