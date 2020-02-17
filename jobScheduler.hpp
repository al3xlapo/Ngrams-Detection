#ifndef JOBSCHEDULER_H
#define JOBSCHEDULER_H

#include "trie.hpp"
#include <pthread.h>


using namespace std;


void* threadFunction(void* jobSch);

class Job{
public:
	uint32_t number;
	int current_version;
	int A_version;
	int D_version;
	char type;
	Trie* t;
	string *ngram;
	int ngram_num;
	int k;
	Min_heap* min_heap;
public:
	Job(uint32_t n,int current_version,int A_version,int D_version, char type, string* ng,int ngram_num,int k, Trie* t, Min_heap * min_heap);
	~Job();
	uint32_t getNumber();
	char getType();
	string* getNgram();
};


typedef struct JobQueueNode{ //a node in the queue of jobs
    public:
        Job* job; //the actual job
        JobQueueNode* next;
}JobQueueNode;


class JobQueue{ //a queue of jobs
public:
	JobQueueNode* head;
	JobQueueNode* tail;
	uint32_t size;
public:
	JobQueue();
	~JobQueue();
	void addJob(Job* j);
	void removeJob();
	JobQueueNode* getFirstJob();
	uint32_t getSize();
	void printQueue();
	bool isEmpty();
};


class JobScheduler{
public:
	uint32_t threadNum; //number of threads in the thread pool, q is auxilliary
	pthread_t* tp; //thread pool
	JobQueue* jq;  //job queue
	char is_dynamic;
	Trie *t;
	string* results;
	int goFlag, qCount;
	pthread_cond_t go;
	pthread_mutex_t mtx;
	pthread_cond_t busy;
	pthread_mutex_t bMtx;
	pthread_mutex_t queueMtx;
	pthread_cond_t nonempty;
public:
	JobScheduler(uint32_t n, Trie *t, char is_dynamic);
	~JobScheduler();
	void submitJob(Job* j);
	void executeAllJobs();
	void waitTasks(uint32_t volley); //waits all submitted tasks in the volley to finish
	//OK_SUCCESS destroy_scheduler( JobScheduler* sch);
};

#endif





