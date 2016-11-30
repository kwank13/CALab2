all: cache

cache: cachesimulator.cpp
	g++ cachesimulator.cpp -o cache

run: all
	./cache cacheconfig.txt trace.txt

test: all
	./cache cacheconfig.txt test.txt

clean:
	rm cache *.out

.PHONY: clean all test run
