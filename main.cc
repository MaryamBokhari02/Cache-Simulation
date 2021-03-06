#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include "funcs.h"
using namespace std;

int main (int argc, char *argv[ ])
{

  //the values that we get from the 
  //command line will be put into these
  //variables. we can use them later in the program
  //like for checking if they have valid values
		
  const int address_bits = 24;		//***************************
  int offset_bits = 0;
  int index_bits = 0;				//ALL NUMBER OF BITS IN BINARY
  int tag_bits = 0;					//***************************

  int cache_capacity = 0;
  int cache_blocksize = 0;
  int cache_associativity = 0;
  int n_cacheblocks = 0;

  bool split = false;
  char* hitpolicy; char* misspolicy;
  bool wb = false, wt = false, wa = false, wn = false;

  char* filename;
  if(!parseParams(argc, argv, cache_capacity, cache_blocksize, cache_associativity, split, hitpolicy,misspolicy, filename))
	  exit(2);

  offset_bits = offsetBits(cache_blocksize);
 // cout << "offset: " << offset_bits << endl;
  index_bits = indexBits(cache_capacity, cache_blocksize, cache_associativity);
  tag_bits = tagBits(address_bits, offset_bits, index_bits);
  //cout << "tag: " << tag_bits << endl;
  n_cacheblocks = nBlocks(cache_capacity, cache_blocksize);

  if (hitpolicy[0] == 't')								//init write policies
	  wt++;
  else if (hitpolicy[0] == 'b')
      wb++;

  if (misspolicy[0] == 'a')
      wa++;
  else if (misspolicy[0] == 'n')
      wn++;

  int* memory = new int [memory_size];					//create + init memory
  for (int i = 0; i < memory_size - 4; i+=4) {
	  string address = int_to_hex(i);
	  if (address!="0")
		  address = pad8bit(address.substr(2));			//substr to remove 0x
	  else
		  address = pad8bit(address);

	  for (int j = 0, start = 0; j < 4; j++) {
		  memory[i+j] = hex_to_int(address.substr(start, 2));
		  start += 2;
	  }
  }

  int evicted = 0; bool open = false;

  if (!split) {											//simulating unified cache
	  	 int misses = 0; int hits = 0;
	  	 Cache cache(n_cacheblocks, tag_bits, cache_blocksize, cache_associativity);
         open = trace_reading(filename, cache, memory, tag_bits, index_bits, offset_bits, hitpolicy, misspolicy, evicted);
         if (open == false)
        	 return 0;

         cout << "STATISTICS:\n";
         ifstream f; f.open("unifiedStats.txt");
         if (f.is_open())
        	 cout << f.rdbuf();
         f.close();
         cout << "Number of Dirty Blocks Evicted From L1D Cache: " << evicted;
         cout << endl << endl;

         cout << "L1 CACHE CONTENTS:"<<endl;
         cout << "SetNumber  Valid  Tag  Dirty  ";
         for (int i = 0; i < cache_blocksize/4; i++) {
       		cout<<"Word"<<i<<"     ";
   		 }
         cout << endl;
         cache.displayInMain();

         cout << "MAIN MEMORY: \nAddress    Words";
         for (int i = 4242560; i < 4246656; i++)		//display 1024 words of memory
         {
       		if (i % 4 == 0)
       			cout << " ";
       	    if (i % 32 == 0) {
       	    	cout << endl;
       	        cout << hex << i << "     ";
       	    }

       	    cout << hex << memory[i];
         }

         cout << endl;
  }

  else {												//simulating split cache
	     offset_bits = offsetBits(cache_blocksize);
	     index_bits = indexBits(cache_capacity/2, cache_blocksize, cache_associativity);
	     n_cacheblocks = nBlocks(cache_capacity/2, cache_blocksize);
	     tag_bits = tagBits(address_bits, offset_bits, index_bits);

	     int missesI = 0; int hitsI = 0; int missesD = 0; int hitsD = 0;

	     Cache Icache(n_cacheblocks, tag_bits, cache_blocksize, cache_associativity);
	     Cache Dcache(n_cacheblocks, tag_bits, cache_blocksize, cache_associativity);
         open = trace_reading2(filename, Dcache, Icache, memory, tag_bits, index_bits, offset_bits, hitpolicy, misspolicy, evicted);
         if (open == false)
        	 return 0;

         cout << "STATISTICS:\n";
         ifstream f; f.open("splitStats.txt");
         if (f.is_open())
        	 cout << f.rdbuf();
         f.close();
         cout << "Number of Dirty Blocks Evicted From L1 Cache: " << evicted;
         cout << endl << endl;

         cout << "L1 DATA CACHE CONTENTS:" << endl;
         cout << "SetNumber  Valid  Tag  Dirty  ";
         for (int i = 0; i < cache_blocksize/4; i++) {
       		cout<<"Word"<<i<<"  ";
   		 }
         cout << endl;
         Dcache.displayInMain();

         cout << "L1 INSTRUCTION CACHE CONTENTS:" << endl;
         cout << "SetNumber  Valid  Tag  ";
         for (int i = 0; i < cache_blocksize/4; i++) {
        	 cout<<"Word"<<i<<"     ";
         }
         cout << endl;
         Icache.displayInMain();

         cout << "MAIN MEMORY: \nAddress    Words";
         for (int i = 4242560; i < 4246656; i++)		//display 1024 words of memory
         {
       		if (i % 4 == 0)
       			cout << " ";
       	    if (i % 32 == 0) {
       	    	cout << endl;
       	        cout << hex << i << "     ";
       	    }

       	    cout << hex << memory[i];
         }
         cout << endl;
  }

  return 0;

}

