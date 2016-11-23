all: cache

cache: cachesimulator.cpp
	g++ cachesimulator.cpp -o cache

test: all
	./cache cacheconfig.txt trace.txt

clean:
	rm cache

.PHONY: clean all test
