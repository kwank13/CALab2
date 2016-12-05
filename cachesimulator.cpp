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
	bool valid;// = false;
};

class cache {

	public:
	int blocksize, ways, cachesize;
	int tagBits, setBits, offsetBits, indexnum;
	struct cacheblock **cb; //Pointer to cache array
	int * counter;

	//Calculate cache properties and initialize values
	cache (int block, int assoc, int size) {
		//counter = 0;
		blocksize = block;
		//ra = NULL;
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
		counter = new int[indexnum];
		for(int i=0; i<indexnum; i++){
            counter[i] = 0;
		}
		cb = new cacheblock*[indexnum]; //create cache array
		for(int i =0; i < indexnum; i++){
            cb[i] = new cacheblock[ways];
		}
		//cb = new cacheblock[indexnum][ways];

	}

	~cache() {
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
	bool readAccess(bitset<32> waddr) {
		long setnum = getSet(waddr);
		bool found = false;
		//Search all ways for matching tag
		for (int i = 0; i < ways; i++){
           // cout << endl << "i=" << i<<"waddr: " << waddr << ", setnum" << setnum<< endl;
			found = compareTag(waddr, cb[setnum][i].addr, tagBits);
            if(found)
                break;
        }
		//cout << endl << "call to find. waddr: " << waddr << ", setnum: " << setnum << ", waccess: " << waccess << ", found: " << found << ", ra: " << ra;
		return found;

	}

	void getAddrFromL2toL1(bitset<32> addr){
	    //bool AddrInL1 = false;
	    if(!fillEmptyWay(addr)){
            this->evictAddrfromL1(addr);
	    }
	    return;
	}

	//Check if a way is empty (not valid data)
	//If empty, write new value and return true, else return false
	bool fillEmptyWay (bitset<32> addr) {
		long setnumber = getSet(addr);
		bool updated = false;

		for (int i = 0; i < ways; i++) {
			if (cb[setnumber][i].valid == false) {
				cb[setnumber][i].addr = addr;
				cb[setnumber][i].valid = true;
				updated = true;
				break;
			}
		}

		return updated;
	}


	//If this is called, no empty ways (as defined in code below)
	//Evict way matching current counter and write new value
	//Update counter and return evicted data
	 void evictAddrfromL1(bitset<32> addr){

		long snum = getSet(addr);
		int counterValue = counter[snum];
		cb[snum][counterValue].addr = addr;

		//cout << ", eaddr: " << eaddr << ", replace: "<< replace << ", snum: " << snum << ", evictedData: "<<evictedData;
		//cout << ", cb[snum*ways+counter].dirty: " << cb[snum*ways+counter].dirty << ", cb[snum*ways+counter].addr";
		//cout << cb[snum][counter].addr;// << ", evict: " << evict;
        updateCounter(snum);
		return;
	}

	void updateCounter(long setIndex) {
		if (counter[setIndex] == ways-1)
			counter[setIndex] = 0;
		else
			counter[setIndex]++;
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
            //cout << endl << line;
            istringstream iss(line);
            if (!(iss >> accesstype >> xaddr)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);


           // access the L1 and L2 Cache according to the trace;
            if (accesstype.compare("R")==0){
                 //Implement by you:
                 // read access to the L1 Cache,
                 //  and then L2 (if required),
                 //  update the L1 and L2 access state variable;

                cout << accessnum << " - read: ";
                //Check for address in L1 cache
                //If found, print status and update L1/L2 states as appropriate
                if (l1cache.readAccess(accessaddr)) {
                    cout << "L1: hit L2: no access" << endl;
                    L1AcceState = RH; L2AcceState = NA;
                } else { //If not found in L1 cache
                    cout << "L1: miss";
                    L1AcceState = RM; //Update L1 state
						//Check if in L2 cache (forward read to next level)
                    if (l2cache.readAccess(accessaddr)) { //Found in L2 cache
                        cout << " L2: hit" << endl;
                        L2AcceState = RH; //Update L2 state
                        l1cache.getAddrFromL2toL1(accessaddr);

                    } else { //Not found in L2
                        cout << " L2: miss" << endl;
                        L2AcceState = RM; //Update L2 state
                        //Write to empty way or evict and then write
                        l2cache.getAddrFromL2toL1(accessaddr);
                        l1cache.getAddrFromL2toL1(accessaddr);

                    }
                }
            }else{
                //Implement by you:
                // write access to the L1 Cache,
                //and then L2 (if required),
                //update the L1 and L2 access state variable;

                cout << accessnum << " - write: ";
                if (l1cache.readAccess(accessaddr)) { //Write hit in L1
                    cout << "L1: hit L2: no access" << endl;
                    L1AcceState = WH; L2AcceState = NA;
                } else { //Write miss in L1
                    cout << "L1: miss";
                    L1AcceState = WM;
                    if (l2cache.readAccess(accessaddr)) { //Write hit in L2
                        cout << " L2: hit" << endl;
                        L2AcceState = WH;
                    } else { //Write miss in L2
                        cout << " L2: miss" << endl;
                        L2AcceState = WM;
						}
                }
            }







                  /*
            for (int i = 0; i < 10; i++){
                            cout << endl;
                for(int j = 0; j < l2cache.ways; j++)
				cout << l2cache.cb[i][j].addr << endl;
				//cout << "[91][" << i << "] valid: " << l2cache.cb[91*l2cache.ways+i].valid << endl;
			}
			for (int i = 0; i < 10; i++){
                    cout << endl;
                for(int j = 0; j < l1cache.ways; j++)
				cout << l1cache.cb[i][j].addr << endl;
				//cout << "[91][" << i << "] valid: " << l2cache.cb[91*l2cache.ways+i].valid << endl;
			}*/

					 //Code to prevent going through full trace file
					///////// REMOVE BEFORE SUBMITTING //////////
					//if (accessnum > 11560)
                      //  break;
                      //cin.get();

             accessnum++;
            // if(accessnum > 5198)
              //  cin.get();
             //system("pause");


/*
			//Prints out contents of cache
			for (int i = 0; i < l2cache.indexnum; i++){
                for(int j = 0; j < l2cache.ways; j++)
				cout << l2cache.cb[i][j].addr << endl;
				//cout << "[91][" << i << "] valid: " << l2cache.cb[91*l2cache.ways+i].valid << endl;
			}
			for (int i = 0; i < l1cache.indexnum; i++){
                for(int j = 0; j < l1cache.ways; j++)
				cout << l1cache.cb[i][j].addr << endl;
				//cout << "[91][" << i << "] valid: " << l2cache.cb[91*l2cache.ways+i].valid << endl;
			}
			//cout << "[183][0]: " << l1cache.cb[183*l1cache.ways+0].addr << endl;

*/

            tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;


        }
        traces.close();
        tracesout.close();
        cout << endl<< "**********THAT'S ALL FOLKS**********"<< endl;
    }
    else cout<< "Unable to open trace or traceout file ";







    return 0;
}
