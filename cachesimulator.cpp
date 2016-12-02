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
	bool dirty;// = false;
	bool valid;// = false;
};

class cache {

	public:
	int blocksize, ways, cachesize;
	int tagBits, setBits, offsetBits, indexnum, counter;
	struct cacheblock *cb; //Pointer to cache array
	bitset<32> *ra; //Pointer to address - used in find() to pass back read hits
    bitset<32> evictedData;  //pointer to address that is being evicted

	//Calculate cache properties and initialize values
	cache (int block, int assoc, int size) {
		counter = 0;
		blocksize = block;
		if(assoc == 0){
            offsetBits = log(blocksize)/log(2);
            tagBits = 32-offsetBits;
            setBits = 0;
            cachesize = size*1024;
            ways = cachesize/blocksize;
            indexnum = 1;
		}else{
            ways = assoc;
            //int temp = (int)(log(size*1000)/log(2)) + 1; //find true cache size exponent
            //cachesize = (int)pow(2, temp); //true cache size
            cachesize = size*1024;
            offsetBits = log(blocksize)/log(2);
            indexnum = (cachesize/blocksize)/ways; //# of sets
            setBits = (int)(log(indexnum)/log(2));
            tagBits = 32 - (offsetBits + setBits);
		}
		cb = new cacheblock[ways*indexnum]; //create cache array
		//cb = new cacheblock[indexnum][ways];

	}

	~cache() {
		delete [] cb;
	}

	//Take in address and return set value as long
	long getSet(bitset<32> iaddr){
		//bitset<32> temp;
		bitset<32> temp;
		int start = offsetBits;
		int end = offsetBits + setBits;

		for (int i = start, j = 0; i < end; i++, j++){
			temp[j] = iaddr[i];
		}

		return temp.to_ulong();
	}

	//Take in address and check all ways for matching tag.
	//If found return true, else return false
	//If write access, then set dirty bit true
	bool find (bitset<32> waddr, bool waccess) {
		long setnum = getSet(waddr);
		bool found = false;

		//Search all ways for matching tag
		for (int i = 0; i < ways; i++){
			found = compareTag(waddr, cb[setnum*ways+i].addr, tagBits);//comparing the tag bits
			if (found && cb[setnum*ways+i].valid == true) { //if found matching tag and data is valid
				//If write operation, update dirty bit
				if (waccess) {
					cb[setnum*ways+i].dirty = true;
					//cb[setnum*(1+i)].dirty = true;
				} else {
					//For read operations, return found address
					ra = &cb[setnum*ways+i].addr;
					//ra = &cb[setnum*(1+i)].addr;
				}
				break;
			}
		}
		return found;
	}

	//Check if a way is empty (not valid data)
	//If empty, write new value and return true, else return false
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

	//If this is called, no empty ways (as defined in code below)
	//Evict way matching current counter and write new value
	//Update counter and return evicted data
	 bool saveEvicted(bitset<32> eaddr, bitset<32> replace) {
		long snum = getSet(eaddr);
		bool evict = false;
		//bitset<32> evicted = cb[snum*ways+counter].addr;
		if(cb[snum*ways+counter].dirty == true){
            evictedData = cb[snum*ways+counter].addr;
            evict = true;
            cb[snum*ways+counter].dirty = false;
		}
		cb[snum*ways+counter].addr = replace;
		updateCounter();

		return evict;
	}

	void updateCounter() {
		if (counter < ways)
			counter++;
		else
			counter = 0;
	}
};

//Compare tags from given addresses
bool compareTag(bitset<32> addr1, bitset<32> addr2, int tagBits){
	bitset<32> tag1, tag2;
	int start = (32-tagBits);

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

	//Initialize L1 cache from config and print stats
	cache l1cache(cacheconfig.L1blocksize, cacheconfig.L1setsize, cacheconfig.L1size);
	cout << "L1: block size = " << l1cache.blocksize << ", sets = " << l1cache.indexnum << ", ways = " << l1cache.ways << ", size = " << l1cache.cachesize << endl;
	cout << "offset bits: " << l1cache.offsetBits << ", set bits: " << l1cache.setBits << ", tag bits: ";
	cout << l1cache.tagBits << endl;

/*
	// Use this code to ensure L1 hit for at least first write access
	l1cache.cb[183*l1cache.ways + 0].addr = 0xbf98454c;//0xbf9845b8;
	l1cache.cb[183*l1cache.ways + 0].valid = true;
	cout << "[183][0] = " << l1cache.cb[183*l1cache.ways+0].addr << endl;
*/

	//Initialize L2 cache from config and print stats
	cache l2cache(cacheconfig.L2blocksize, cacheconfig.L2setsize, cacheconfig.L2size);
	cout << "L2: block size = " << l2cache.blocksize << ", sets = " << l2cache.indexnum << ", ways = " << l2cache.ways << ", size = " << l2cache.cachesize << endl;
   	cout << "offset bits: " << l2cache.offsetBits << ", set bits: " << l2cache.setBits << ", tag bits: ";
	cout << l2cache.tagBits << endl;

/*
	// Use this code to ensure L2 hit for at least first write access
	l2cache.cb[91*l2cache.ways + 3].addr = //0xbf98454c;// 0xbf9845b8;
	l2cache.cb[91*l2cache.ways + 3].valid = true;
	//cout << "[91][3] = " << l2cache.cb[91*l2cache.ways+3].addr << endl;
*/

	/*
	//Use this block of code to test eviction
	for (int i = 0; i < l2cache.ways; i++){
		l2cache.cb[91*l2cache.ways+i].addr = 0xffffffff;
		l2cache.cb[91*l2cache.ways+i].valid = true;
	}
	for (int i = 0; i < l2cache.ways; i++){
		cout << "[91][" << i << "]: " << l2cache.cb[91*l2cache.ways+i].addr << endl;
		//cout << "[91][" << i << "] valid: " << l2cache.cb[91*l2cache.ways+i].valid << endl;
	}
	l1cache.cb[183*l1cache.ways+0].addr = 0xffffffff;
	l1cache.cb[183*l1cache.ways+0].valid = true;
	cout << "[183][0]: " << l1cache.cb[183*l1cache.ways+0].addr << endl;
	*/

	int accessnum = 1; //Keep track of accesses



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

					cout << accessnum << " - read: ";
					//Check for address in L1 cache
					//If found, print status and update L1/L2 states as appropriate
					if (l1cache.find(accessaddr, false)) {
						cout << "L1: hit L2: no access" << endl;
						L1AcceState = RH; L2AcceState = NA;
					} else { //If not found in L1 cache
						cout << "L1: miss";
						L1AcceState = RM; //Update L1 state
						bitset<32> update;
						bitset<32> spare;
						//Check if in L2 cache (forward read to next level)
						if (l2cache.find(accessaddr, false)) { //Found in L2 cache
							cout << " L2: hit" << endl;
							L2AcceState = RH; //Update L2 state
							update = *l2cache.ra; //Get data from L2 cache
							//Write to empty way in L1 or evict and then write
							if (!l1cache.find_empty_way(accessaddr, update)) {
								if(l1cache.saveEvicted(accessaddr, update)){ //updating data in L1 and checking if the data being evicted is dirty or not
                                    l2cache.find(l2cache.evictedData, true);//if evicted data is dirty, then there is write access to L2
                                }
								// << "L1 spare: " << spare << endl;
								//l2cache.find(spare, true); //Perform write operation for evicted data
							}
						} else { //Not found in L2
							cout << " L2: miss" << endl;
							L2AcceState = RM; //Update L2 state
							//Write to empty way or evict and then write
							if (!l1cache.find_empty_way(accessaddr, accessaddr)) {
								if(l1cache.saveEvicted(accessaddr, accessaddr)){ //updating data in L1 and checking if the data being evicted is dirty or not
                                    l2cache.find(l2cache.evictedData, true);//if evicted data is dirty, then there is write access to L2
                                }
							}
							if (!l2cache.find_empty_way(accessaddr, accessaddr)) {
								if(l2cache.saveEvicted(accessaddr, accessaddr)){
                                    cout << "save evicted data in main memory if dirty bit is set"<< endl;
                                    //cout << "Evicted Data from L2: " << *l2cache.evictedData << endl;
								} //Save to main mem (don't need to do anything in sim)

							}

						}
					}





                 }
             else
             {
                   //Implement by you:
                  // write access to the L1 Cache,
                  //and then L2 (if required),
                  //update the L1 and L2 access state variable;

					cout << accessnum << " - write: ";
					if (l1cache.find(accessaddr, true)) { //Write hit in L1
						cout << "L1: hit L2: no access" << endl;
						L1AcceState = WH; L2AcceState = NA;
					} else { //Write miss in L1
						cout << "L1: miss";
						L1AcceState = WM;
						if (l2cache.find(accessaddr, true)) { //Write hit in L2
							cout << " L2: hit" << endl;
							L2AcceState = WH;
						} else { //Write miss in L2
							cout << " L2: miss" << endl;
							L2AcceState = WM;
						}
					}

					 //Code to prevent going through full trace file
					///////// REMOVE BEFORE SUBMITTING //////////
					//if (accessnum > 150)
						//break;






                  }
             accessnum++;

			/*
			//Prints out contents of cache
			for (int i = 0; i < l2cache.ways; i++){
				cout << "[91][" << i << "]: " << l2cache.cb[91*l2cache.ways+i].addr << endl;
				//cout << "[91][" << i << "] valid: " << l2cache.cb[91*l2cache.ways+i].valid << endl;
			}
			cout << "[183][0]: " << l1cache.cb[183*l1cache.ways+0].addr << endl;
			*/


            tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;


        }
        traces.close();
        tracesout.close();
    }
    else cout<< "Unable to open trace or traceout file ";







    return 0;
}
