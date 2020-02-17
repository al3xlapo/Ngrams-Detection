#include <inttypes.h>

typedef struct {
     unsigned long bit_size;
     int number_of_hash;
     unsigned char *a;
} Bloom;


Bloom * Bloom_create(unsigned long size,int hash_num);/*dhmiourgia tou bloom filter*/

void Bloom_destroy(Bloom *b);/*apodesmeush ths mnhmhs tou bloom filter*/

void reset(Bloom *b);

void turn_on_bit(Bloom *b,uint64_t h); /*metatroph se 1 tou bit pou 8a prokupsei analogws me thn timh */ 
                                      /*ths antistoixhs hash function*/
int check_bit(Bloom *b,uint64_t h);/*elegxos enos sugkekrimenou bit kai epistrogh 1 an auto einai 1 */
                                   /*alliws epistrofh 0*/
void add_word(const char *word,int len, Bloom *b);/*pros8hkh lekshs sto bloom filter*/

int check_word(const char *word,int len, Bloom *b);/*elegxos an h leksh "word" anhkei sto bloom filter*/
