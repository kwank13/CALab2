/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size)  t=32-s-b
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>

using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss




struct config{
       int L1blocksize;
       int L1setsize;
       int L1size;
       int L2blocksize;
       int L2setsize;
       int L2size;
       };

/* you can define the cache class here, or design your own data structure for L1 and L2 cache
class cache {
      
      }
*/       

struct cacheblock {
	bitset<32> addr;
	bool dirty;
	bool valid;
};

class cache {

	public:
	int blocksize, assoc, cachesize;
	int tagBits, setBits, offsetBits, indexnum, counter;
	struct cacheblock *addr;

	cache (int block, int ways, int size) {
		counter = 0;
		blocksize = block;
		assoc = ways;
		int temp = (int)(log(size*1000)/log(2)) + 1;
		cachesize = (int)pow(2, temp);
		offsetBits = log(blocksize)/log(2);
		indexnum = (cachesize/blocksize)/assoc;
		cout << "indexnum: " << indexnum << endl;
		setBits = (int)(log(indexnum)/log(2));
		tagBits = 32 - (offsetBits + setBits);

		addr = new cacheblock[assoc*indexnum];
	}

	~cache() {
		delete [] addr;
	}
};

/*
class L2 {
	public:
	int blocksize, assoc, cachesize;
	int tagBits, setBits, offsetBits, indexnum;
	bitset<32> *addr;

	L2(int block, int ways, int size) {
		blocksize = block;
		assoc = ways;
		int temp = (int)(log(size*1000)/log(2)) + 1;
		cachesize = (int)pow(2, temp);
		offsetBits = log(blocksize)/log(2);
		indexnum = (cachesize/blocksize)/assoc;
		cout << "indexnum: " << indexnum << endl;
		setBits = (int)(log(indexnum)/log(2));
		tagBits = 32 - (offsetBits + setBits);

		addr = new bitset<32>[assoc*indexnum];
//
		addr = new bitset<32>*[assoc];
		for (int i = 0; i < assoc; i++)
			addr[i] = new bitset<32>[indexnum];
//
	}

	~L2() {
		delete [] addr;
//
		for (int i = 0; i < assoc; i++)
			delete [] addr[i];

		delete [] addr;
//
	}
};
*/

bool compareTag(bitset<32> addr1, bitset<32> addr2, int tagBits){
	bitset<32> tag1, tag2;
	int start = (32-tagBits) + 1;

	for (int i = start, j = 0; i < 32; i++, j++){
		tag1[j] = addr1[i];
		tag2[j] = addr2[i];
	}

	if (tag1 == tag2)
		return true;
	else
		return false;
}

int main(int argc, char* argv[]){

	if (argc != 3) {
		printf("Usage: cache <cacheconfig> <trace file>\n");
		return 0;
	}

    
    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    cache_params.open(argv[1]);
    while(!cache_params.eof())  // read config file
    {
      cache_params>>dummyLine;
      cache_params>>cacheconfig.L1blocksize;
      cache_params>>cacheconfig.L1setsize;              
      cache_params>>cacheconfig.L1size;
      cache_params>>dummyLine;              
      cache_params>>cacheconfig.L2blocksize;           
      cache_params>>cacheconfig.L2setsize;        
      cache_params>>cacheconfig.L2size;
      }
    
  
  
   // Implement by you: 
   // initialize the hirearch cache system with those configs
   // probably you may define a Cache class for L1 and L2, or any data structure you like

	cache l1cache(cacheconfig.L1blocksize, cacheconfig.L1setsize, cacheconfig.L1size);
	cout << "offset bits: " << l1cache.offsetBits << ", set bits: " << l1cache.setBits << ", tag bits: ";
	cout << l1cache.tagBits << endl;

	l1cache.addr[1*l1cache.indexnum + 0] = 0xbf984000;
	cout << "[1][0] = " << l1cache.addr[1*l1cache.indexnum+0] << endl;
	cache l2cache(cacheconfig.L2blocksize, cacheconfig.L2setsize, cacheconfig.L2size);
   	cout << "offset bits: " << l2cache.offsetBits << ", set bits: " << l2cache.setBits << ", tag bits: ";
	cout << l2cache.tagBits << endl;


   
   
  int L1AcceState =0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
  int L2AcceState =0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;
   
   
    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";
    
    traces.open(argv[2]);
    tracesout.open(outname.c_str());
    
    string line;
    string accesstype;  // the Read/Write access type from the memory trace;
    string xaddr;       // the address from the memory trace store in hex;
    unsigned int addr;  // the address from the memory trace store in unsigned int;        
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;
    
    if (traces.is_open()&&tracesout.is_open()){    
        while (getline (traces,line)){   // read mem access file and access Cache
            
            istringstream iss(line); 
            if (!(iss >> accesstype >> xaddr)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);
           
           
           // access the L1 and L2 Cache according to the trace;
              if (accesstype.compare("R")==0)
              
             {    
                 //Implement by you:
                 // read access to the L1 Cache, 
                 //  and then L2 (if required), 
                 //  update the L1 and L2 access state variable;

                 //cout << "access address: " << accessaddr << endl;
                 
                 
                 
                 
                 
                 
                 }
             else 
             {    
                   //Implement by you:
                  // write access to the L1 Cache, 
                  //and then L2 (if required), 
                  //update the L1 and L2 access state variable;
                  
				  //bool equal = compareTag(l1cache.addr[1*l1cache.indexnum+0], accessaddr, l1cache.tagBits); 
				  //cout << "equal: " << equal << endl;
                  
                  
                  
                  
                  
                  }
              
              
             
            tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;
             
             
        }
        traces.close();
        tracesout.close(); 
    }
    else cout<< "Unable to open trace or traceout file ";


   
    
  

   
    return 0;
}
