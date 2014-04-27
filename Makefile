# Standard flags
CPPFLAGS = -std=gnu++11 -O3 -pthread -g3 -Wall -c -fmessage-length=0 -MMD

# Python build
PYTHON = /usr/include/python2.7
BOOST_INC = /usr/include

all:
# Compile classes
	g++ $(CPPFLAGS) -MF"prngs/GlibcRand.d" -MT"prngs/GlibcRand.d" -o "prngs/GlibcRand.o" "./prngs/GlibcRand.cpp"
	g++ $(CPPFLAGS) -MF"prngs/Mt19937.d" -MT"prngs/Mt19937.d" -o "prngs/Mt19937.o" "./prngs/Mt19937.cpp"
	g++ $(CPPFLAGS) -MF"prngs/Ruby.d" -MT"prngs/Ruby.d" -o "prngs/Ruby.o" "./prngs/Ruby.cpp"
	g++ $(CPPFLAGS) -MF"PRNGFactory.d" -MT"PRNGFactory.d" -o "PRNGFactory.o" "./PRNGFactory.cpp"

# Make the binary
	g++ $(CPPFLAGS) -MF"untwister.d" -MT"untwister.d" -o "untwister.o" "./untwister.cpp"
	g++ -std=gnu++11 -O3 -pthread -o "untwister" ./prngs/GlibcRand.o ./prngs/Mt19937.o ./prngs/Ruby.o ./PRNGFactory.o ./untwister.o

python:
# Compile the classes
	g++ $(CPPFLAGS) -MF"prngs/GlibcRand.d" -MT"prngs/GlibcRand.d" -o "prngs/GlibcRand.o" "./prngs/GlibcRand.cpp"
	g++ $(CPPFLAGS) -MF"prngs/Mt19937.d" -MT"prngs/Mt19937.d" -o "prngs/Mt19937.o" "./prngs/Mt19937.cpp"
	g++ $(CPPFLAGS) -MF"prngs/Ruby.d" -MT"prngs/Ruby.d" -o "prngs/Ruby.o" "./prngs/Ruby.cpp"
	g++ $(CPPFLAGS) -MF"PRNGFactory.d" -MT"PRNGFactory.d" -o "PRNGFactory.o" "./PRNGFactory.cpp"

# Make the shared object
	g++ -std=c++11 -I$(PYTHON) -I$(BOOST_INC) -c -fPIC py-untwister.cpp -o py-untwister.o
	g++ -std=c++11 -shared ./prngs/GlibcRand.o ./prngs/Mt19937.o ./prngs/Ruby.o ./PRNGFactory.o py-untwister.o \
		-lboost_python -lpython2.7 -o untwister.so

clean:
	rm -f ./prngs/*.o
	rm -f ./prngs/*.d
	rm -f *.o
	rm -f *.d
	rm -f untwister untwister.so
