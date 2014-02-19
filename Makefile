all:
	g++ -Wall -O3 -g -std=gnu++11 untwister.cpp Generator.cpp -o untwister
clean:
	rm -f untwister
