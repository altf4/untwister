all:
	g++ -Wall -g -std=gnu++11 untwister.cpp Generator.cpp -o untwister
clean:
	rm -f untwister
