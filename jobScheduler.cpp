#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <pthread.h>

#include "jobScheduler.hpp"

using namespace std;

//Thread Function

void* threadFunction(void* jobSch){
	JobScheduler* js = (JobScheduler*)jobSch;
	
	if ( js->is_dynamic == 'n') //static
	{
		while(1){
			pthread_mutex_lock(&js->queueMtx);
			while(!js->goFlag || js->jq->isEmpty()) //wait till insertions are done
				pthread_cond_wait(&js->nonempty, &js->queueMtx);
			JobQueueNode* jN = js->jq->getFirstJob(); //get the first job to be done and then remove it from the queue
			js->jq->removeJob();
			Job* j = jN->job;		
			
					
			if(j->type == 'Q'){
				Bloom * filter = Bloom_create(100000,5);
				int i = j->ngram_num;
				string* out = new string();
				
				
				for(int x=0;x<i;x++){	
					j->t->Static_SearchNgram(&j->ngram[x], i-x, out, filter, j->min_heap);
				}
				pthread_mutex_unlock(&js->queueMtx);
				if(out->empty()){				
					js->results[j->number-1] = "-1\n";
				}
				else{
					*out = out->substr(1, out->size()-2);
					js->results[j->number - 1] = *out+"\n";
				}
				
				reset(filter);
				delete out;			
				delete[] j->ngram; //actually delete jobQueueNode, job and its contents
				delete j;
				delete jN;
				Bloom_destroy(filter);
			}

			pthread_mutex_lock(&js->bMtx); //when qCount reaches 0 and busy mtx is signaled the waitAllTasks will know all jobs are finished
			js->qCount--;
			pthread_cond_signal(&js->busy);
			pthread_mutex_unlock(&js->bMtx);
		}

	}
	else { //dynamic
		while(1){

			pthread_mutex_lock(&js->queueMtx);
			while(!js->goFlag || js->jq->isEmpty()) //wait till insertions are done
				pthread_cond_wait(&js->nonempty, &js->queueMtx);
			JobQueueNode* jN = js->jq->getFirstJob(); //get the first job to be done and then remove it from the queue
			js->jq->removeJob();
			Job* j = jN->job;		
			pthread_mutex_unlock(&js->queueMtx);

			if(j->type == 'A')
			{
				//cout<<"A::"<<j->current_version<< ", " << j->A_version <<"word = "<<j->ngram<<endl;
				//pthread_mutex_lock(&js->queueMtx);
				//j->t->InsertNgram(j->ngram,j->ngram_num,j->k);
				//pthread_mutex_unlock(&js->queueMtx);
			}
			else if (j->type == 'D')
			{
				//cout<<"D::"<<j->current_version <<", " <<j->D_version<<"word = "<<j->ngram<<endl;
				//pthread_mutex_lock(&js->queueMtx);
				//j->t->DeleteNgram(j->ngram,j->ngram_num);
				//pthread_mutex_unlock(&js->queueMtx);
			}
			else if(j->type == 'Q'){
				Bloom * filter = Bloom_create(100000,5);
				int i = j->ngram_num;
				string* out = new string();
				
				
				for(int x=0;x<i;x++){	
					j->t->SearchNgram(&j->ngram[x], i-x, out, filter, j->min_heap);
				}
				
				if(out->empty()){				
					js->results[j->number-1] = "-1\n";
				}
				else{
					*out = out->substr(1, out->size()-2);
					js->results[j->number - 1] = *out+"\n";
				}
				
				reset(filter);
				delete out;			
				delete[] j->ngram; //actually delete jobQueueNode, job and its contents
				delete j;
				delete jN;
				Bloom_destroy(filter);
			}

			pthread_mutex_lock(&js->bMtx); //when qCount reaches 0 and busy mtx is signaled the waitAllTasks will know all jobs are finished
			js->qCount--;
			pthread_cond_signal(&js->busy);
			pthread_mutex_unlock(&js->bMtx);
			}
		}		
}


//Job Class Functions

Job::Job(uint32_t n, int current_version,int A_version,int D_version, char type, string* ng, int ngram_num, int k, Trie* t, Min_heap* min_heap){
	number = n;
	this->type = type; 
	this->ngram_num = ngram_num;
	ngram = new string[150000];
	for(int j=0; j<ngram_num; j++)
		ngram[j] = ng[j];
	this->t = t;
	this->min_heap = min_heap;
	this->k = k;
}


Job::~Job(){
	//delete [] ngram;
}


uint32_t Job::getNumber(){
	return number;
}

char Job::getType(){
	return type;
}

string* Job::getNgram(){
	return ngram;
}


//JobQueue Class Functions

JobQueue::JobQueue(){
	head = NULL;
	tail = NULL;
	size = 0;
}

JobQueue::~JobQueue(){
    JobQueueNode *tmp;
    while(head!=NULL){
	tmp=head;
	head=head->next;
	//free(tmp->job);
	delete tmp;
    }
}


void JobQueue::addJob(Job* j){ //add a new job to the queue
 
    JobQueueNode *tmp = new JobQueueNode();
    tmp->job = j;
    tmp->next = NULL;
    if(head==NULL){ // if queue is empty
    	head=tmp; // new node is the head
    }
    else
        tail->next=tmp; // connects to previous nodes
    size++;
    tail=tmp;  // new node is the tail
}


void JobQueue::removeJob(){ //remove a job from the queue
    size--;
    JobQueueNode* tmp;
    if(head==NULL){
        cout << "JobQueue is empty!" << endl;
    }
    else{
        tmp=head;  // set temporary node to be the head
        head=head->next;  // set the new head to the next node
        //delete tmp;
    }	
}


void JobQueue::printQueue(){
	JobQueueNode* j = head;
	while(j != NULL){
		cout << j->job->number << endl;
		j = j->next;
	}
}

JobQueueNode* JobQueue::getFirstJob(){
	if(head!=NULL)
		//return head->job; //return the job of JobQueueNode
		return head; //get queueNode
	else
        	return NULL;	
}


bool JobQueue::isEmpty(){
    return (head==NULL);
}

uint32_t JobQueue::getSize(){
	return size;
}


//JobScheduler Class Functions


JobScheduler::JobScheduler(uint32_t n, Trie *t, char is_dynamic){

	threadNum = n;
	qCount = 0;
	goFlag = 0;
	tp = new pthread_t[n];
	jq = new JobQueue();
	this->t = t;
	this->is_dynamic = is_dynamic;
	uint32_t i;

	pthread_cond_init(&go, 0);
	pthread_mutex_init(&mtx, 0);
	pthread_cond_init(&busy, 0);
	pthread_mutex_init(&bMtx, 0);
	pthread_cond_init(&nonempty, 0);
	pthread_mutex_init(&queueMtx, 0);

	JobScheduler* js = this;
	for(i=0; i<n; i++)
		pthread_create(&tp[i], NULL, threadFunction, (void*)this);
}


JobScheduler::~JobScheduler(){
	uint32_t i;
	//delete tp;
	delete jq;
	pthread_mutex_destroy(&queueMtx);
	pthread_cond_destroy(&nonempty);
	pthread_mutex_destroy(&mtx);
	pthread_cond_destroy(&go);
	pthread_mutex_destroy(&bMtx);
	pthread_cond_destroy(&busy);
}


void JobScheduler::submitJob(Job* j){
	pthread_mutex_lock(&queueMtx); //get the mutex
	jq->addJob(j); //add the job to the queue
	qCount++;
	pthread_mutex_unlock(&queueMtx); //unlock the mutex */
}


void JobScheduler::executeAllJobs(){

	
	pthread_mutex_lock(&queueMtx); //wake up threads
	//cout<<"execute"<<endl;
	goFlag = 1;
	pthread_cond_broadcast(&nonempty); 
	pthread_mutex_unlock(&queueMtx);
}

void JobScheduler::waitTasks(uint32_t volley){
	pthread_mutex_lock(&bMtx);
	while(qCount>0) //wait till the threads finished their job
		pthread_cond_wait(&busy, &bMtx);
	goFlag=0; //set the flag for the next volley
	//cout<<"over and queue has "<<jq->size<<" elements"<<endl;
	pthread_mutex_unlock(&bMtx);
}




