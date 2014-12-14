# Standard flags
CPPFLAGS = -std=gnu++11 -O3 -g3 -Wall -c -fmessage-length=0 -MMD -fPIC
PYTHON = /usr/include/python2.7
BOOST = /usr/include
OBJS = ./prngs/LSBState.o ./prngs/GlibcRand.o ./prngs/Mt19937.o ./prngs/Ruby.o ./prngs/PRNGFactory.o ./Untwister.o
TEST_OBJS = ./tests/runner.o ./tests/TestRuby.o ./tests/TestMt19937.o ./tests/TestPRNGFactory.o ./tests/TestUntwister.o

all: GlibcRand Mt19937 RubyRand LSBState PRNGFactory Untwister
	# Make the cli binary
	g++ $(CPPFLAGS) -pthread -MF"main.d" -MT"main.d" -o "main.o" "./main.cpp"
	g++ -std=gnu++11 -O3 -pthread $(OBJS) main.o -o untwister

python: GlibcRand Mt19937 RubyRand LSBState PRNGFactory Untwister
	# Make the shared object
	g++ $(CPPFLAGS) -I$(PYTHON) -I$(BOOST) py-untwister.cpp -o py-untwister.o
	g++ -std=c++11 -shared -fPIC -O3 $(OBJS) py-untwister.o -lboost_python -lpython2.7 -o untwister.so

tests: GlibcRand Mt19937 RubyRand LSBState PRNGFactory Untwister
	g++ $(CPPFLAGS) -MF"./tests/TestRuby.d" -MT"./tests/TestRuby.d" -o "./tests/TestRuby.o" "./tests/TestRuby.cpp"
	g++ $(CPPFLAGS) -MF"./tests/TestMt19937.d" -MT"./tests/TestMt19937.d" -o "./tests/TestMt19937.o" "./tests/TestMt19937.cpp"
	g++ $(CPPFLAGS) -MF"./tests/TestPRNGFactory.d" -MT"./tests/TestPRNGFactory.d" -o "./tests/TestPRNGFactory.o" "./tests/TestPRNGFactory.cpp"
	g++ $(CPPFLAGS) -MF"./tests/TestUntwister.d" -MT"./tests/TestUntwister.d" -o "./tests/TestUntwister.o" "./tests/TestUntwister.cpp"
	g++ $(CPPFLAGS) -MF"./tests/runner.d" -MT"./tests/runner.d" -o "./tests/runner.o" "./tests/runner.cpp"
	g++ -std=gnu++11 -O3 -pthread $(OBJS) $(TEST_OBJS) -o untwister_tests -lcppunit

GlibcRand:
	g++ $(CPPFLAGS) -MF"prngs/GlibcRand.d" -MT"prngs/GlibcRand.d" -o "prngs/GlibcRand.o" "./prngs/GlibcRand.cpp"

Mt19937:
	g++ $(CPPFLAGS) -MF"prngs/Mt19937.d" -MT"prngs/Mt19937.d" -o "prngs/Mt19937.o" "./prngs/Mt19937.cpp"

RubyRand:
	g++ $(CPPFLAGS) -MF"prngs/Ruby.d" -MT"prngs/Ruby.d" -o "prngs/Ruby.o" "./prngs/Ruby.cpp"

LSBState:
	g++ $(CPPFLAGS) -MF"prngs/LSBState.d" -MT"prngs/LSBState.d" -o "prngs/LSBState.o" "./prngs/LSBState.cpp"

PRNGFactory:
	g++ $(CPPFLAGS) -MF"prngs/PRNGFactory.d" -MT"prngs/PRNGFactory.d" -o "prngs/PRNGFactory.o" "./prngs/PRNGFactory.cpp"

Untwister:
	g++ $(CPPFLAGS) -pthread -MF"Untwister.d" -MT"Untwister.d" -o "Untwister.o" "./Untwister.cpp"


clean:
	rm -f ./prngs/*.o
	rm -f ./prngs/*.d
	rm -f ./tests/*.o
	rm -f ./tests/*.d
	rm -f *.o
	rm -f *.d
	rm -f untwister untwister_tests untwister.so
