#include <iostream>
#include <cstring>
#include <iomanip>
#include <string.h>
using namespace std;

extern const int memory_size;

bool parseParams(int argc, char *argv[ ], int& cache_capacity,
				int& cache_blocksize, int& cache_associativity, bool& split, char*& hitpolicy, char*& misspolicy, char*& tracefile);
int offsetBits(int blocksize);
int indexBits(int capacity, int blocksize, int a);
int tagBits(int adrs, int offsetBits, int indexBits);
int nBlocks(int capacity, int blocksize);
int nDatablocks(int blocksize);
int str_to_int(string a);
string int_to_hex(int dec);
int hex_to_int(string hex);			//can receive hex string like "0x3f", "3f", "3F", "00003f" etc
int bin_to_int(string bin);
string hex_to_bin(const string& hexaDecimal);
string pad8bit(string a);
string pad6bit(string a);
string trim(string hex);			//trims "0x" from hex string
void splitAddress(string hexAddress, int tag_bits, int index_bits, int offset_bits, 
		int& index, string& tag, int& offset);			//splits address into index, tag, offset
int find_nth(string str, int n);

class CacheBlock {
protected:
	int lru;						//1 if least recently used, 2 to a otherwise
	int dirty; 				   		//0 if Dirty, 1 otherwise
	int v;
	char* tag;
	string* datablocks;
	int n_datablocks;
public:
	CacheBlock() {
		v = 0;
		tag = NULL;
		datablocks = NULL;
		n_datablocks = 0;
		dirty = 0;
		lru = 0;
	}
	
	void setlru(int num)
	{
		lru = num;
	}

	int getlru()
	{
		return lru;
	}

	int getD()
	{
		return dirty; 
	}
	
	void setD() 
	{
		dirty=1; 
	}

	void unsetD() {
		dirty=0;
	}

	int getV() {
		return v;
	}
	
	void setV() {
		v = 1;
	}

	char* getTag() {
		return tag;
	}
	
	void setTag(string t) {
		tag = new char[t.length()+1];
		strcpy(tag, t.c_str());
		//delete
	}

	string* getDataBlocks() {
		return datablocks;
	}

	int n_DataBlocks() {
		return n_datablocks;
	}

	void displayDataBlocks() {
		for (int i = 0; i < n_datablocks; i++) {
			cout << datablocks[i] << " | ";
		}
	}

	void displayDataBlocksInMain() {
		for (int i = 0; i < n_datablocks; i+=4) {
			for (int j = 0; j < 4; j++) {
				if (datablocks[i+j] == " ")
					cout << "00";
				else
					cout << trim(datablocks[i+j]);
			}
			cout << "  ";
		}
	}

	void initializeTag(int tag_bits) {
		tag = new char[tag_bits];
		for (int i=0; i<tag_bits; i++)
			tag[i] = 'x';
	}

	void initializeDataBlocks(int n_datablocks) {
		this->n_datablocks = n_datablocks;
		datablocks = new string[n_datablocks];
		for (int i = 0; i < n_datablocks; i++)
			datablocks[i] = " ";
	}

	~CacheBlock() {
		if (tag)
			delete[] tag;
		if (datablocks) {
			delete[] datablocks;
		}
	}
};

class Cache {
private:
	CacheBlock** cacheblocks;					//size of cacheblocks = number of blocks (defined in main)/set size
	int size;
	int a;										//set size
	int n_blocks;								//capacity/blocksize

public:
	Cache (int n_blocks, int tag_bits, int n_datablocks, int cache_associativity) {

		a = cache_associativity;
		this->n_blocks = n_blocks;
		size = n_blocks / a;

		cacheblocks = new CacheBlock* [size];
		for (int i = 0; i < size; i++) {			//for every set initialize tag and data of cache line
			cacheblocks[i] = new CacheBlock[a];
			for (int j = 0; j < a; j++) {
				cacheblocks[i][j].initializeTag(tag_bits);
				cacheblocks[i][j].initializeDataBlocks(n_datablocks);
			}
		}

	}

	int geta()
	{
		return a;
	}

	CacheBlock** getCacheblocks() {
		return cacheblocks;
	}

	bool search(int index, const char* tag, int& cacheLine, bool LRU = false) {			//returns true if hit
		if (index < 0 || index >= size)				//checking if index is valid
			return false;

		bool hit = false;

		int i;								        //index of last available space in case of miss
		for (i = 0; i < a; i++) {
			if ( (cacheblocks[index][i].getV() == 1) && strcmp(cacheblocks[index][i].getTag(), tag) == 0 )
			{
				cacheLine = i;
				hit = true;
				break;
			}
			
			else if (cacheblocks[index][i].getV() == 0)
			{
				cacheLine = i;
				break;
			}
			
			else if ((cacheblocks[index][i].getV() == 1) && strcmp(cacheblocks[index][i].getTag(), tag) != 0)
			{
                cacheLine = i;
			}
		}

		if (i == a)
			LRU++;		//if cache full, LRU=true, replacement needed
		return hit;
	}
	
	int updateCache(int index, int cacheLine, int offset, string strval, bool LRU) {
		if( LRU == true && a>1)
		{
			int location, currentlru;				//find lru=1 of the cacheline value and update cache there
			for( int i=0 ; i<a ; i++ )
			{
				currentlru = cacheblocks[index][i].getlru();
				if( currentlru == 1 )
					location = i;
			}
			for (int i=0, start=0; i < 4; i++)
			{
				cacheblocks[index][location].getDataBlocks()[offset + i] = strval.substr(start,2);
				start += 2;
			}
			LRU_Val_Update(index, location);
		}

		else
		{
			LRU_Val_Update(index, cacheLine);
			for (int i=0, start=0; i < 4; i++)
			{
				cacheblocks[index][cacheLine].getDataBlocks()[offset + i] = strval.substr(start,2);
				start += 2;
			}
		}
	}

	void LRU_Val_Update( int index, int cacheLine )
	{
		if (cacheblocks[index][cacheLine].getlru() != a) {			//if MRU accessed again, don't decrement
			cacheblocks[index][cacheLine].setlru(a);
			int currentlru;
			for( int i=0 ; i<a ; i++ )
			{
				currentlru = cacheblocks[index][i].getlru();
				if( i != cacheLine && currentlru != 0 && currentlru != 1)
				{
					cacheblocks[index][i].setlru(currentlru - 1);	//else set LRU
				}
			}
		}
	}

	bool wordEmpty(int index, int cacheLine, int offset) {
		int count = 0;
		for (int i = 0; i < 4; i++) {
			if (offset+i < cacheblocks[index]->n_DataBlocks() && cacheblocks[index][cacheLine].getDataBlocks()[offset+i] == " ")
				count++;
		}
		if (count == 4) {
			return true;
		}
		else
			return false;
	}

	void display() {
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < a; j++) {
				cout << i << ". LRU: " << cacheblocks[i][j].getlru() << ", v: " << cacheblocks[i][j].getV() << ", tag: "
						<< cacheblocks[i][j].getTag() << ", data: ";
								cacheblocks[i][j].displayDataBlocks(); cout << endl;
			}
			cout << endl;
		}
	}

	void displayInMain() {
		for (int i = 0; i < size; i++) {
					for (int j = 0; j < a; j++) {
						cout << hex << i << "          " << cacheblocks[i][j].getV() << "      ";
		                if (cacheblocks[i][j].getTag()[0] == 'x')
		                    cout << 'x';
		                else {
		                    string str(cacheblocks[i][j].getTag());
		                    cout << hex << (bin_to_int(str));
		                }
		                cout << "     " << cacheblocks[i][j].getD(); cout << "     ";
		                cacheblocks[i][j].displayDataBlocksInMain(); cout << endl;
					}
					cout << endl;
				}
	}

	~Cache () {
		if (cacheblocks) {
			for (int i = 0; i < size; i++)
				delete[] cacheblocks[i];
			delete[] cacheblocks;
		}
	}
};

//read trace file for unified cache
bool trace_reading(char * tracename, Cache& cache, int* memory, int tag_bits, int index_bits, int offset_bits,
		char* hitpolicy, char* misspolicy, int& evicted);
//read trace file for split cache
bool trace_reading2(char * tracename, Cache& cache,Cache& Icache, int* memory, int tag_bits, int index_bits, int offset_bits,
                   char* hitpolicy, char* misspolicy, int& evicted);
//write to memory
void writeMemory(Cache& cache, int* memArr, string address,int tag_bits, int index_bits, int offset_bits,
              string strval,char* hitpolicy, char* misspolicy, float& whits, float& wmisses, int& evicted, int& accesscount);
//read from memory
void readMemory(Cache& cache, int* memArr, string address, 
		int tag_bits, int index_bits, int offset_bits, float& rhits, float& rmisses, char* hitpolicy, int& evicted);
