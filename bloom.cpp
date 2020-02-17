#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <inttypes.h>
#include "bloom.hpp"
#include "bloom_hash.hpp"

using namespace std;

Bloom * Bloom_create(unsigned long size,int hash_num){/*desmeush size bytes gia thn dhmiourgia tou bloom filter*/
     Bloom *b;
     b= (Bloom *) malloc(sizeof(Bloom));
     b->a= (unsigned char *) malloc(size*sizeof(unsigned char));
     b->bit_size=size*CHAR_BIT;
     b->number_of_hash=hash_num;
     memset(b->a,0,size);
return b;

}

void Bloom_destroy(Bloom *b){/*apodesmeush mnhmhs tou bloom filter*/
    free(b->a);
    free(b);
}

void reset(Bloom *b){
	memset(b->a,0,b->bit_size/CHAR_BIT);
}

void turn_on_bit(Bloom *b,uint64_t h){

    unsigned long k, byte_pos, bit_pos;
    k = h % b->bit_size; /*metatroph ths timhs ths hash function analogws ton eurws twn 8esewn*/
    byte_pos = k / CHAR_BIT;/*h 8esh ston pinaka tou bloom*/                                 
    bit_pos = k % CHAR_BIT;/*to bit sthn sugkekrimenh 8esh tou pinaka*/
    *(b->a+byte_pos) |= (128>>bit_pos);/*olis8ish tou 128(10000000) oses 8eseis xreiazontai deksia kai */
}                                       /*pros8esh gia na ginei to bit 1 sth 8esh bit_pos*/

int check_bit(Bloom *b,uint64_t h){ /*elegxos gia to an ena sugkekrimeno bit einai 1*/
                                    /*an nai tote epistrofh 1 alliws 0*/
    unsigned long k, byte_pos, bit_pos;
    k = h % b->bit_size;
    byte_pos = k / CHAR_BIT;
    bit_pos = k % CHAR_BIT;
    int c;
    c = *(b->a+byte_pos) & (128>>bit_pos);
    if(c!=0){ /*an ena bit einai diaforo tou 0 tote o ari8mos 8a einai diaforo tou 0*/
         return 1;
    }
    else{
         return 0;
    }
}

void add_word(const char *word,int len, Bloom *b){
    
    int i;
    uint64_t hash1 = MurmurHash2(word,len,0);
    uint64_t hash2 = MurmurHash2(word,len,hash1);
    for(i=0;i<b->number_of_hash;i++){
       turn_on_bit(b,hash1+i*hash2);
    }
}

int check_word(const char *word,int len, Bloom *b){
    int flag=1;
    int i;
    uint64_t hash1 = MurmurHash2(word,len,0);
    uint64_t hash2 = MurmurHash2(word,len,hash1);
    for(i=0;i<b->number_of_hash;i++){
       flag=flag * check_bit(b,hash1+i*hash2);
    }
    return flag;
}


