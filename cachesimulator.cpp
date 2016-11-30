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
bool compareTag(bitset<32> addr1, bitset<32> addr2, int tagBits);

//Struct for individual cache entries
struct cacheblock {
	bitset<32> addr;
	bool dirty = false;
	bool valid = false;

	cacheblock& operator= (const cacheblock& a) {
		addr = a.addr;
		dirty = a.dirty;
		valid = a.valid;

		return *this;
	}
};

class cache {

	public:
	int blocksize, ways, cachesize;
	int tagBits, setBits, offsetBits, indexnum, counter;
	struct cacheblock *cb;
	bitset<32> *ra;

	//Calculate cache properties and initialize values
	cache (int block, int assoc, int size) {
		counter = 0;
		blocksize = block;
		ways = assoc;
		int temp = (int)(log(size*1000)/log(2)) + 1; //find true cache size exponent
		cachesize = (int)pow(2, temp); //true cache size
		offsetBits = log(blocksize)/log(2);
		indexnum = (cachesize/blocksize)/ways; //# of sets
		//cout << "indexnum: " << indexnum << endl;
		setBits = (int)(log(indexnum)/log(2));
		tagBits = 32 - (offsetBits + setBits);

		cb = new cacheblock[ways*indexnum]; //create cache array
	}

	~cache() {
		delete [] cb;
	}

	//Take in address and return set value as long
	long getSet(bitset<32> iaddr){
		//cout << "getSet input: " << iaddr << endl;
		bitset<32> temp;
		int start = offsetBits;
		int end = offsetBits + setBits;
		//cout << "start: " << start << ", end: " << end << endl;

		//cout << "getSet = ";

		for (int i = start, j = 0; i < end; i++, j++){
			temp[j] = iaddr[i];
			//cout << iaddr[i];
		}
		//cout << endl;
/*
		for (int i = start; i < end; i++)
			cout << iaddr[i];
		cout << endl;
*/

		//cout << "getSet = " << temp << endl;
		return temp.to_ulong();
	}

	//Take in address and check all ways for matching tag.
	//If found return true, else return false
	//If write access, then set dirty bit true
	bool find (bitset<32> waddr, bool waccess) {
		//cout << "write input: " << waddr << endl;
		long setnum = getSet(waddr);
		bool found = false;
		//cout << "setnum: " << setnum << endl;

		for (int i = 0; i < ways; i++){
			found = compareTag(waddr, cb[setnum*ways+i].addr, tagBits);
			//cout << "[" << setnum << "][" << i << "]: " << this->cb[setnum*ways+i].addr << endl;
			//cout << "written: " << written << endl;
			//cout << "loop " << i << ": " << written << endl;
			if (found && cb[setnum*ways+i].valid == true) {
				if (waccess) {
					cb[setnum*ways+i].dirty = true;
					//cout << "Setting dirty bit" << endl;
				} else {
					ra = &cb[setnum*ways+i].addr;
				}
				break;
				//cout << "written: " << written << endl;
				//cout << "dirty: " << cb[setnum*ways+i].dirty << endl;
				//return true;
			}
			/*
			else
				return false;
			*/
		}
		return found;
		//return true;
	}

	bool find_empty_way (bitset<32> old_addr, bitset<32> new_addr) {
		long setnumber = getSet(old_addr);
		bool updated = false;

		for (int i = 0; i < ways; i++) {
			if (cb[setnumber*ways+i].valid == false) {
				cb[setnumber*ways+i].addr = new_addr;
				cb[setnumber*ways+i].valid = true;
				updated = true;
				break;
			}
		}

		return updated;
	}

	void updateCounter() {
		if (counter < ways)
			counter++;
		else
			counter = 0;
	}
};

bool compareTag(bitset<32> addr1, bitset<32> addr2, int tagBits){
	bitset<32> tag1, tag2;
	int start = (32-tagBits);
	//cout << "compareTag start: " << start << endl;

	for (int i = start, j = 0; i < 32; i++, j++){
		tag1[j] = addr1[i];
		tag2[j] = addr2[i];
	}

	//cout << "compareTag tag1: " << tag1 << ", tag2: " << tag2 << endl;
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
	cout << "L1: block size = " << l1cache.blocksize << ", sets = " << l1cache.indexnum << ", ways = " << l1cache.ways << ", size = " << l1cache.cachesize << endl;
	cout << "offset bits: " << l1cache.offsetBits << ", set bits: " << l1cache.setBits << ", tag bits: ";
	cout << l1cache.tagBits << endl;

	//l1cache.cb[1*l1cache.indexnum + 0].addr = 0xbf984000;
	//cout << "[1][0] = " << l1cache.cb[1*l1cache.indexnum+0].addr << endl;
/*
	// Use this code to ensure L1 hit for at least first write access
	l1cache.cb[183*l1cache.ways + 0].addr = 0xbf9845b8;
	l1cache.cb[183*l1cache.ways + 0].valid = true;
	cout << "[183][0] = " << l1cache.cb[183*l1cache.ways+0].addr << endl;
*/
	cache l2cache(cacheconfig.L2blocksize, cacheconfig.L2setsize, cacheconfig.L2size);
	cout << "L2: block size = " << l2cache.blocksize << ", sets = " << l2cache.indexnum << ", ways = " << l2cache.ways << ", size = " << l2cache.cachesize << endl;
   	cout << "offset bits: " << l2cache.offsetBits << ", set bits: " << l2cache.setBits << ", tag bits: ";
	cout << l2cache.tagBits << endl;
//
	// Use this code to ensure L2 hit for at least first write access
	l2cache.cb[91*l2cache.ways + 3].addr = 0xbf9845b8;
	l2cache.cb[91*l2cache.ways + 3].valid = true;
	cout << "[91][1] = " << l2cache.cb[91*l2cache.ways+3].addr << endl;
//

	int accessnum = 1;
	bool equal = false;

   
   
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
                 
    			  //equal = compareTag(l1cache.cb[1*l1cache.indexnum+0].addr, accessaddr, l1cache.tagBits); 
				  //cout << accessnum << ": " << equal << endl;
					cout << accessnum << " - read: ";
					if (l1cache.find(accessaddr, false)) {
						cout << "L1: hit L2: no access" << endl;
						L1AcceState = RH; L2AcceState = NA;
					} else {
						cout << "L1: miss";
						L1AcceState = RM;
						if (l2cache.find(accessaddr, false)) {
							cout << " L2: hit" << endl;
							L2AcceState = RH;
							//cout << "RA: " << *l2cache.ra << endl;
							bitset<32> update = *l2cache.ra;
							bitset<32> spare;
							//Pseudo code
							if (l1cache.find_empty_way(accessaddr)) {
								l1cache.write(accessaddr, update);
							} else {
								spare = l1cache.evict(accessaddr);
								l1cache.write(accessaddr, update);
								l2cache.find(spare, true);
							}
						} else {
							cout << " L2: miss" << endl;
							L2AcceState = RM;
							if (l2cache.find_empty_way(accessaddr)) {
								l2cache.write(accessaddr, accessaddr);
							} else {
								spare = l2cache.evict(accessaddr);
								l2cache.write(accessaddr, accessaddr);
						}
					}
             
                 
                 
                 
                 
                 }
             else 
             {    
                   //Implement by you:
                  // write access to the L1 Cache, 
                  //and then L2 (if required), 
                  //update the L1 and L2 access state variable;
                  
				  //equal = compareTag(l1cache.cb[183*l1cache.ways+0].addr, accessaddr, l1cache.tagBits); 
				  //cout << accessnum << ": " << equal << endl;
				  //equal = compareTag(l2cache.cb[183*l2cache.ways+0].addr, accessaddr, l2cache.tagBits);
				  //cout << accessnum << ": " << equal << endl;
					cout << accessnum << " - write: ";
					if (l1cache.find(accessaddr, true)) {
						cout << "L1: hit L2: no access" << endl;
						L1AcceState = WH; L2AcceState = NA;
					} else {
						cout << "L1: miss";
						L1AcceState = WM;
						if (l2cache.find(accessaddr, true)) {
							cout << " L2: hit" << endl;
							L2AcceState = WH;
						} else {
							cout << " L2: miss" << endl;
							L2AcceState = WM;
						}
					}
					if (accessnum > 15)
						break;
                  
                  
                  
                  
                  
                  }
             accessnum++; 
			/*
			for (int i = 0; i < l2cache.ways; i++){
				cout << "[183][" << i << "]: " << l2cache.cb[183*l2cache.ways+i].addr << endl;
			}*/
              
             
            tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;
             
             
        }
        traces.close();
        tracesout.close(); 
    }
    else cout<< "Unable to open trace or traceout file ";


   
    
  

   
    return 0;
}
