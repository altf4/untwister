# Standard flags
CPPFLAGS = -std=gnu++11 -O3 -pthread -g3 -Wall -c -fmessage-length=0 -MMD -fPIC
PYTHON = /usr/include/python2.7
BOOST_INC = /usr/include


all: GlibcRand Mt19937 RubyRand LSBState PRNGFactory
	# Make the binary
	g++ $(CPPFLAGS) -MF"untwister.d" -MT"untwister.d" -o "untwister.o" "./untwister.cpp"
	g++ -std=gnu++11 -O3 -pthread -o "untwister" ./prngs/LSBState.o ./prngs/GlibcRand.o ./prngs/Mt19937.o ./prngs/Ruby.o ./PRNGFactory.o ./untwister.o

GlibcRand:
	g++ $(CPPFLAGS) -MF"prngs/GlibcRand.d" -MT"prngs/GlibcRand.d" -o "prngs/GlibcRand.o" "./prngs/GlibcRand.cpp"

Mt19937:
	g++ $(CPPFLAGS) -MF"prngs/Mt19937.d" -MT"prngs/Mt19937.d" -o "prngs/Mt19937.o" "./prngs/Mt19937.cpp"

RubyRand:
	g++ $(CPPFLAGS) -MF"prngs/Ruby.d" -MT"prngs/Ruby.d" -o "prngs/Ruby.o" "./prngs/Ruby.cpp"

LSBState:
	g++ $(CPPFLAGS) -MF"prngs/LSBState.d" -MT"prngs/LSBState.d" -o "prngs/LSBState.o" "./prngs/LSBState.cpp"

PRNGFactory:
	g++ $(CPPFLAGS) -MF"PRNGFactory.d" -MT"PRNGFactory.d" -o "PRNGFactory.o" "./PRNGFactory.cpp"
	g++ $(CPPFLAGS) -MF"untwister.d" -MT"untwister.d" -o "untwister.o" "./untwister.cpp"
	g++ -std=gnu++11 -O3 -pthread -o "untwister" ./prngs/LSBState.o ./prngs/GlibcRand.o ./prngs/Mt19937.o ./prngs/Ruby.o ./PRNGFactory.o ./untwister.o


python: GlibcRand Mt19937 RubyRand LSBState PRNGFactory
	# Make the shared object
	g++ $(CPPFLAGS) -I$(PYTHON) -I$(BOOST_INC) py-untwister.cpp -o py-untwister.o
	g++ -std=c++11 -shared -fPIC -O3 ./prngs/LSBState.o ./prngs/GlibcRand.o ./prngs/Mt19937.o ./prngs/Ruby.o ./PRNGFactory.o py-untwister.o \
		-lboost_python -lpython2.7 -o untwister.so


clean:
	rm -f ./prngs/*.o
	rm -f ./prngs/*.d
	rm -f *.o
	rm -f *.d
	rm -f untwister untwister.so
