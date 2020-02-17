CC = g++
CPPFLAGS = -g -pthread
HEADERS = jobScheduler.hpp trie.hpp bloom.hpp bloom_hash.hpp min_heap.hpp trieNode.hpp hash.hpp
SRC =  jobScheduler.cpp main.cpp trie.cpp bloom.cpp bloom_hash.cpp min_heap.cpp trieNode.cpp hash.cpp
OBJS =  jobScheduler.o main.o trie.o bloom.o bloom_hash.o min_heap.o trieNode.o hash.o

all: ngrams

%.o: %.c $(HEADERS)
	$(CC) -c $< $ $

ngrams: $(OBJS)
	$(CC) $(CPPFLAGS) -o $@ $(OBJS)

clean:
	rm -f *.o ngrams
