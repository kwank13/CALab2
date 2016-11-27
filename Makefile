all: cache

cache: cachesimulator.cpp
	g++ cachesimulator.cpp -std=c++11 -o cache

test: all
	./cache cacheconfig.txt trace.txt

clean:
	rm cache

.PHONY: clean all test
