# Standard flags
CPPFLAGS = -std=gnu++11 -O3 -pthread -g3 -Wall -c -fmessage-length=0 -MMD

# Compile classes
all: glibcrand mt19937 ruby LSBState PRNGfactory
	# Make the binary
	g++ $(CPPFLAGS) -MF"untwister.d" -MT"untwister.d" -o "untwister.o" "./untwister.cpp"
	g++ -std=gnu++11 -O3 -pthread -o "untwister" ./prngs/LSBState.o ./prngs/GlibcRand.o ./prngs/Mt19937.o ./prngs/Ruby.o ./PRNGFactory.o ./untwister.o

glibcrand:
	g++ $(CPPFLAGS) -MF"prngs/GlibcRand.d" -MT"prngs/GlibcRand.d" -o "prngs/GlibcRand.o" "./prngs/GlibcRand.cpp"

mt19937:
	g++ $(CPPFLAGS) -MF"prngs/Mt19937.d" -MT"prngs/Mt19937.d" -o "prngs/Mt19937.o" "./prngs/Mt19937.cpp"

ruby:
	g++ $(CPPFLAGS) -MF"prngs/Ruby.d" -MT"prngs/Ruby.d" -o "prngs/Ruby.o" "./prngs/Ruby.cpp"

LSBState:
	g++ $(CPPFLAGS) -MF"prngs/LSBState.d" -MT"prngs/LSBState.d" -o "prngs/LSBState.o" "./prngs/LSBState.cpp"

PRNGfactory:
	g++ $(CPPFLAGS) -MF"PRNGFactory.d" -MT"PRNGFactory.d" -o "PRNGFactory.o" "./PRNGFactory.cpp"

clean:
	rm -f ./prngs/*.o
	rm -f ./prngs/*.d
	rm -f untwister untwister.o untwister.d PRNGFactory.o PRNGFactory.d
