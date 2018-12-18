#include <stdio.h>
#include <unistd.h> //for the getopt function
#include <stdlib.h> //for atoi
#include <math.h>   //for log2 and ceil
#include <fstream>
#include <sstream>
#include "funcs.h"
#include <iostream>
using namespace std;

const int memory_size = 16777216;

bool parseParams(int argc, char *argv[ ], int& cache_capacity,
                int& cache_blocksize, int& cache_associativity, bool& split, char*& hitpolicy, char*& misspolicy, char*& tracefile)
{

  //needed for the parsing of command line options
  int c;
  bool c_flag, b_flag, a_flag;
  bool errflg = false;
  char* hitcheck = NULL; char* misscheck = NULL;
  int checker = 0;
  c_flag = b_flag = a_flag = errflg = false;

  //the following variables are used by getopt and
  //are defined elsewhere so we just make them
  //extern here
  extern char *optarg;
  extern int optind, optopt;

  //start the parsing of the command line options.
  //end is indicated by getopt returning -1
  //each option has a case statement
  //the corresponding flags are set if the option exists on
  //the command line
  //the : int he getopt indicates that the option preceeding the
  //: requires a argument to be specified
  while ((c = getopt(argc, argv, "w:w:sc:b:a:")) != -1) {
    switch (c) {
      case 'w':
    	  if (checker == 0)
    	  {
    		  hitcheck = optarg;
        	if (hitcheck[0] == 'b' || hitcheck[0] == 't' /*|| check[0] == 'a' || check[0] == 'n'*/)
        	{
            	hitpolicy = hitcheck;
        	}
        	else
        	{
            	errflg++; cout << "invalid hit policy\n";
        	}
        	checker++;
    	  }

    	  else if (checker == 1)
    	  {
    		  misscheck = optarg;
    		  if (misscheck[0] == 'a' || misscheck[0] == 'n' /*|| check[0] == 'a' || check[0] == 'n'*/)
    		  {
    			  misspolicy = misscheck;
    		  }
    		  else
    		  {
    			  errflg++; cout << "invalid miss policy\n";
    		  }
    	  }

        break;

      case 's':
        split = true;
        break;

      case 'c':
        cache_capacity = atoi(optarg);
        if (cache_capacity == 4 || cache_capacity == 8 || cache_capacity == 16 || cache_capacity == 32 || cache_capacity == 64/*yahan*/ || cache_capacity == 128 || cache_capacity == 256)
            c_flag = true;
        break;
      case 'b':
        cache_blocksize = atoi(optarg);
        if (cache_blocksize == 4 || cache_blocksize == 8 || cache_blocksize == 16 || cache_blocksize == 32 || cache_blocksize == 64 || cache_blocksize == 128 || cache_blocksize == 256 || cache_blocksize == 512 /*yahan*/||
        		cache_blocksize == 1024 || cache_blocksize == 2048 || cache_blocksize == 4096)
            b_flag = true;
        break;
      case 'a':
        cache_associativity = atoi(optarg);
        if (cache_associativity == 1 || cache_associativity == 2 || cache_associativity == 4 || cache_associativity == 8 || cache_associativity == 16 /*yahan*/||
        		cache_associativity == 32 || cache_associativity == 64 || cache_associativity == 128 || cache_associativity == 256 || cache_associativity == 512 || cache_associativity == 1024 ||
				cache_associativity == 2048 || cache_associativity == 4096 || cache_associativity == 8192 || cache_associativity == 16384)
            a_flag = true;
        break;
      case ':':       /* -c without operand */
        fprintf(stderr,"Option -%c requires an operand\n", optopt);
        errflg++;
        break;
      case '?':
        fprintf(stderr, "Unrecognised option: -%c\n", optopt);
        errflg=true;
    }
  }

        if(!hitcheck)
        {
            hitcheck = new char[1]; hitcheck[0] = 'b';
            hitpolicy = hitcheck;
        }

        if (!misscheck)
        {
        	misscheck = new char[1]; misscheck[0] = 'a';
        	misspolicy = misscheck;
        }

        //reading trace file name
        int i = 0;
        for (; i < argc ; i++) {
      	  if (strstr(argv[i], ".trace")) {
      		  tracefile = argv[i];
      		  break;
      	  }
        }


  //check if we have all the options and have no illegal options
  if(errflg || !c_flag || !b_flag || !a_flag) {
    fprintf(stderr, "usage: %s -c<capacity> -b<blocksize> -a<associativity> -w<hit policy> -w<miss policy> <file.trace> > output.txt\n", argv[0]);
    return false;
  }

  return true;
}

int offsetBits(int blocksize) {
	return log2(blocksize);
}

int indexBits(int capacity, int blocksize, int a) {
	return log2( ((pow(2,10)*capacity) / blocksize)/a );
}

int tagBits(int adrs, int offsetBits, int indexBits) {
	return (adrs - offsetBits - indexBits);
}

int nBlocks(int capacity, int blocksize) {					//number of lines in cache
	return ( (pow(2,10)*capacity) / blocksize );
}

int nDatablocks(int blocksize) {							//number of data words in a cache line
	return blocksize / 4;
}

int str_to_int (string s) {
	try {
		return stoi(s);
	} catch(...) {
		return -1;
	}
}

int hex_to_int(string hex) {
	char* s = new char [hex.length()+1];
	strcpy(s, hex.c_str());
	int result;
	sscanf(s, "%x", &result);
	delete[] s;
	return result;
}

string int_to_hex(int dec) {
	char buffer[8]; 
	sprintf ( buffer, "%#x", dec );
	string str(buffer);
	return str;
}

int bin_to_int(string bin) {
    int rem, temp, dec = 0, b = 1;
    temp = str_to_int(bin);

    while (temp > 0)
    {
        rem = temp % 10;
        dec = dec + rem * b;
        b *= 2;
        temp /= 10;
    }
    return dec;
}

string hex_to_bin(const string& hexaDecimal)
{
	string binary = "";
	long int i=0;

	while(i < hexaDecimal.length())
	{
		switch(hexaDecimal[i])
	    {
	    	case '0': binary+="0000"; break;
	        case '1': binary+="0001"; break;
	        case '2': binary+="0010"; break;
	        case '3': binary+="0011"; break;
	        case '4': binary+="0100"; break;
            case '5': binary+="0101"; break;
            case '6': binary+="0110"; break;
	        case '7': binary+="0111"; break;
	        case '8': binary+="1000"; break;
	        case '9': binary+="1001"; break;
	        case 'A': binary+="1010"; break;
	        case 'B': binary+="1011"; break;
            case 'C': binary+="1100"; break;
            case 'D': binary+="1101"; break;
            case 'E': binary+="1110"; break;
            case 'F': binary+="1111"; break;
	        case 'a': binary+="1010"; break;
	        case 'b': binary+="1011"; break;
	        case 'c': binary+="1100"; break;
	        case 'd': binary+="1101"; break;
	        case 'e': binary+="1110"; break;
	        case 'f': binary+="1111"; break;
	        default:  cout<<"nInvalid hexadecimal digit "<<hexaDecimal[i];
	    }
	    i++;
	}
	return binary;
}

string pad6bit(string a)
{
    int len = a.length();

    if (len > 6) {
        cout << "maximum 6 bit hex data accepted.\n" << endl;
        return a;
    }

    string retValue = "";
    if (len < 6) {
        int add = 6 - len;
        string pad = "";
        for (int i = 0; i < add; i++)
            pad += "0";

        retValue = pad + a;
        return retValue;
    }

    if (len == 6) {
        return a;
    }
}

string pad8bit(string a) {
	int len = a.length();
		
	if (len > 8) {
		cout << "maximum 8 bit hex data accepted.\n" << endl;
		return a;
	}
	
	string retValue = "";
	if (len < 8) {
		int add = 8 - len;
		string pad = "";
		for (int i = 0; i < add; i++)
			pad += "0";
		
		retValue = pad + a;
		return retValue;
	}
	
	if (len == 8) {
		return a;
	}
}

string trim(string hex) {
	if (hex.find("0x")==0) {
		return hex.substr(2);
	}
	else {
		return hex;
	}
}

void splitAddress(string hexAddress, int tag_bits, int index_bits, int offset_bits, 
		int& index, string& tag, int& offset) {
	
	if (hexAddress.find("0x") == 0) {
		hexAddress = hexAddress.substr(2);
	}
	
	int len = hexAddress.length();
		
	if (len > 6) {
		cout << "maximum 6 bit hex address accepted.\n" << endl;
		index = -1;
		return;
	}
	
	if (len != 6) {
		int add = 6 - len;
		string pad = "";
		for (int i = 0; i < add; i++)
			pad += "0";
		
		hexAddress = pad + hexAddress;
	}
	
	string binary = hex_to_bin(hexAddress);				//converting the hex into binary
	
	tag = binary.substr(0, tag_bits);
	index = bin_to_int( binary.substr(tag_bits, index_bits) );
	offset = bin_to_int( binary.substr(tag_bits+index_bits, offset_bits) );
}

void readMemory(Cache& cache, int* M, string address, 
		int tag_bits, int index_bits, int offset_bits, float& hitrate, float& missrate, char* hitpolicy, int& evicted) {		//address in hex
	
	int index; string tag; int offset;
    address = pad6bit(address);
	splitAddress(address, tag_bits, index_bits, offset_bits, index, tag, offset);
	//cout << "tag: " << tag << ", index: " << index << ", offset: " << offset << endl;
	
	if (index != -1) {														//if index is valid
		int cacheLine; bool LRU = false;
		bool hit = cache.search(index, tag.c_str(), cacheLine, LRU);		//check if hit or miss
		//bool offsethit = cache.wordEmpty(index, cacheLine, offset);
		
		if (hit) {			//if hit
            hitrate++;
			cache.LRU_Val_Update(index, cacheLine);							//update lru count
		}
		
		else {				//if miss
            missrate++;
			int memIndex = hex_to_int(address);
			
			cache.getCacheblocks()[index][cacheLine].setV();
			cache.getCacheblocks()[index][cacheLine].setTag(tag);
			
			//if overwriting dirty block, move it to memory
			if (hitpolicy[0] == 'b' && cache.getCacheblocks()[index][cacheLine].getD() == 1) {
				evicted++;
				string cache_val = "";
				for (int i=0; i < 4; i++) {												//move to memory
					if (offset+i < cache.getCacheblocks()[index]->n_DataBlocks() && memIndex+i < memory_size) {
						cache_val = cache.getCacheblocks()[index][cacheLine].getDataBlocks()[offset + i];
						M[memIndex+i] = hex_to_int(cache_val);
					}
				}
				cache.getCacheblocks()[index][cacheLine].unsetD();						//set dirty to 0
			}

			if (LRU == true && cache.geta()>1)	//if set full
			{
				int location, currentlru;							//searching for LRU block's location
				for( int i=0 ; i<cache.geta() ; i++ )
				{
					currentlru = cache.getCacheblocks()[index][i].getlru();
					if( currentlru == 1 )
						location = i;
				}

				for (int i = 0; i < 4; i++) {						//writing to LRU block's location
					if (offset+i < cache.getCacheblocks()[index]->n_DataBlocks()  && memIndex+i < memory_size) {
						cache.getCacheblocks()[index][location].getDataBlocks()[offset+i] = int_to_hex(M[memIndex+i]);
					}
				}
				cache.LRU_Val_Update(index, location);				//update lru count
			}

			else				//if set not full
			{
				cache.LRU_Val_Update(index, cacheLine);
				for (int i = 0; i < 4; i++) {						//writing to first empty block
					if (offset+i < cache.getCacheblocks()[index]->n_DataBlocks() && memIndex+i < memory_size) {
						cache.getCacheblocks()[index][cacheLine].getDataBlocks()[offset+i] = int_to_hex(M[memIndex+i]);
					}
				}
			}
		}
	}
}

void writeMemory(Cache& cache, int* memArr, string address, int tag_bits, int index_bits, int offset_bits,
		string strval, char* hitpolicy, char* misspolicy, float& hitrate, float& missrate, int& evicted, int& accesscount) {

	int index; string tag; int offset;
    address = pad6bit(address);
	splitAddress(address, tag_bits, index_bits, offset_bits, index, tag, offset);

	strval = pad8bit(strval);

	if (index != -1) {
		int memIndex = hex_to_int(address);
		int cacheLine = 0; bool LRU = false;
		bool hit = cache.search(index, tag.c_str(), cacheLine, LRU);		//check if hit or miss
		//bool offsethit = cache.wordEmpty(index, cacheLine, offset);

		if (hit) {

			hitrate++;
			if( hitpolicy[0] == 't')		//-wt
			{
				cache.updateCache(index, cacheLine, offset, strval, LRU);	//Updating Value into cache
				for (int i=0, start=0; i < 4; i++) {						//Updating Value into Memory
					if (memIndex+i < memory_size) {
						memArr[memIndex+i] = hex_to_int(strval.substr(start,2));
						start += 2;
					}
				}
				accesscount++;
			}
			else							//default -wb
			{
				if( cache.getCacheblocks()[index][cacheLine].getD() == 0 ) 		//if dirty = 0
				{
					cache.updateCache(index, cacheLine, offset, strval, LRU);	//write to cache
					cache.getCacheblocks()[index][cacheLine].setV();
					cache.getCacheblocks()[index][cacheLine].setD();			//update dirty = 1
				}
				else 															//if dirty = 1
				{
					evicted++;
					string cache_val = "";
					for (int i=0; i < 4; i++) {									//evicting old value
						if (offset+i < cache.getCacheblocks()[index]->n_DataBlocks() && memIndex+i < memory_size) {
							cache_val = cache.getCacheblocks()[index][cacheLine].getDataBlocks()[offset + i];
							memArr[memIndex+i] = hex_to_int(cache_val);
						}
					}
					cache.updateCache(index, cacheLine, offset, strval, LRU);	//writing new value to cache
					accesscount++;
				}
				
			}
		}

		else {
            missrate++;

			if (misspolicy[0] == 'a')		//default miss policy -wa
			{
				//LOAD MISSING BLOCK IN CACHE
				cache.getCacheblocks()[index][cacheLine].setTag(tag);
				cache.getCacheblocks()[index][cacheLine].setV();

				//APPLY HIT POLICY
				if (hitpolicy[0] == 't') {
					cache.updateCache(index, cacheLine, offset, strval, LRU);
					for (int i=0, start=0; i < 4; i++) {
						if (memIndex+i < memory_size) {
							memArr[memIndex+i] = hex_to_int(strval.substr(start,2));
							start += 2;
						}
					}
					accesscount++;
				}

				else if (hitpolicy[0] == 'b') {
					if( cache.getCacheblocks()[index][cacheLine].getD() == 0 )
					{
						cache.updateCache(index, cacheLine, offset, strval, LRU);
						cache.getCacheblocks()[index][cacheLine].setD();
					}
					else
					{
						evicted++;
						string cache_val = "";
						for (int i=0; i < 4; i++) {
							if (offset+i < cache.getCacheblocks()[index]->n_DataBlocks()  && memIndex+i < memory_size) {
								cache_val = cache.getCacheblocks()[index][cacheLine].getDataBlocks()[offset + i];
								memArr[memIndex+i] = hex_to_int(cache_val);
							}
						}
						cache.updateCache(index, cacheLine, offset, strval, LRU);
						accesscount++;
					}
				}
			}

			else		// 2nd miss policy -wn
			{
				for (int i=0,start=0; i < 4; i++) {
					if (memIndex+i < memory_size) {
						memArr[memIndex+i] = hex_to_int(strval.substr(start,2));
						start += 2;
					}
				}
				accesscount++;
			}

		}
	}
}

int find_nth(string str, int n)
{
	int i=0; int pos=0; int from=0; 
	while(i<n )
	{
		pos=str.find(' ',from); 
		from=pos+1; 
		i++; 
	}
	return pos; 
}

bool trace_reading(char* tracename, Cache& cache, int* memory, int tag_bits, int index_bits, int offset_bits,
		char* hitpolicy, char* misspolicy, int& evicted)
{
	ifstream file; 
	file.open(tracename); 
	string line;
	float rmisses = 0; float rhits = 0; float wmisses = 0; float whits = 0;
	int accesscount = 0;
	if(file.is_open() )
	{
		while (getline(file, line) )
		{
			string oper = line.substr(0, line.find(' '));
			stringstream geek(oper); int op=0; 
			geek>>op; 
			if(op == 0 || op == 2)
			{
				string second_token = line.substr( find_nth(line, 1)+1 , find_nth(line, 2)-find_nth(line, 1)-1 );
				readMemory(cache, memory, second_token, tag_bits, index_bits, offset_bits, rhits, rmisses, hitpolicy, evicted);
			}
			else 
			{
				string second_token = line.substr(find_nth(line, 1)+1 , find_nth(line, 2)-find_nth(line, 1) -1);	
				string third_token= line.substr( find_nth(line, 2)+1, find_nth(line, 3)- find_nth(line, 2) -1 ); 
				writeMemory(cache, memory, second_token, tag_bits, index_bits, offset_bits, third_token, hitpolicy,misspolicy, whits, wmisses, evicted, accesscount);
			}
		}

        int digits = 4;
        ofstream stats;
        stats.open("unifiedStats.txt");
        stats << "L1 Misses: \nTotal: " << wmisses+rmisses << " DataReads: " << rmisses << " DataWrites: " << wmisses << "\n";
        stats << "L1 Miss Rate: \nTotal: " << setprecision(digits) << ((wmisses+rmisses)/(wmisses+rmisses+whits+rhits))*100 << "% DataReads: " << setprecision(digits) << (rmisses/(rmisses+rhits))*100 << "% DataWrites: " << setprecision(digits) << (wmisses/(wmisses+whits))*100 << "%\n";
        stats.close();
        return true;
	}

	else
	{
		cout<<"File could not be read "<<endl; 
		return false;
	}	
}

bool trace_reading2(char * tracename, Cache& cache, Cache& Icache, int* memory, int tag_bits, int index_bits, int offset_bits,
                   char* hitpolicy, char* misspolicy, int& evicted)
{
    ifstream file;
    file.open(tracename);
    string line;
    float rmissesI = 0.0; float rhitsI = 0.0; float rmissesD = 0.0; float rhitsD = 0.0;
    float wmissesD = 0.0; float whitsD = 0.0;

    int nInstructions = 0; int nData = 0;

    int accesscount = 0;

    if(file.is_open() )
    {
        while (getline(file, line) )
        {

            string oper = line.substr(0, line.find(' '));
            stringstream geek(oper); int op=0;
            geek>>op;
            if(op == 0 || op == 2)
            {
                string second_token = line.substr( find_nth(line, 1)+1 , find_nth(line, 2)-find_nth(line, 1)-1 );

                if (op == 2) {
                	nInstructions++;
                    readMemory(Icache, memory, second_token, tag_bits, index_bits, offset_bits, rhitsI, rmissesI, hitpolicy, evicted);
                }
                else {
                	nData++;
                    readMemory(cache, memory, second_token, tag_bits, index_bits, offset_bits, rhitsD, rmissesD, hitpolicy, evicted);
                }
            }
            else
            {
            	nData++;
                string second_token = line.substr(find_nth(line, 1)+1 , find_nth(line, 2)-find_nth(line, 1) -1);
                string third_token= line.substr( find_nth(line, 2)+1, find_nth(line, 3)- find_nth(line, 2) -1 );
                writeMemory(cache, memory, second_token, tag_bits, index_bits, offset_bits, third_token, hitpolicy,
                            misspolicy, whitsD, wmissesD, evicted, accesscount);
            }
        }

      //  cout << "total instructions: " << nInstructions << endl;
      //  cout << "total data: " << nData << endl;
        cout << "access count: " << accesscount << endl;

        int digits = 4;
        ofstream stats;
        stats.open("splitStats.txt");
        stats << "L1I Misses: \nTotal: " << rmissesI << " InstructionReads: " << rmissesI << "\n";
        stats << "L1I Miss Rate: \nTotal: " << setprecision(digits) << (rmissesI/(rmissesI+rhitsI))*100 << "% InstructionReads: " << setprecision(digits) << (rmissesI/(rmissesI+rhitsI))*100 << "%\n";
        stats << "\n";
        stats << "L1D Misses: \nTotal: " << wmissesD+rmissesD << " DataReads: " << rmissesD << " DataWrites: " << wmissesD << "\n";
        stats << "L1D Miss Rate: \nTotal: " << setprecision(digits) << ((wmissesD+rmissesD)/(wmissesD+rmissesD+whitsD+rhitsD))*100 << "%  DataReads: " << setprecision(digits) << (rmissesD/(rmissesD+rhitsD))*100 << "% DataWrites: " << setprecision(digits) << (wmissesD/(wmissesD+whitsD))*100 << "%\n";
        stats.close();
        return true;
    }

    else
    {
        cout<<"File could not be read "<<endl;
        return false;
    }
}


