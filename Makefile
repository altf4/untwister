# Standard flags
CPPFLAGS = -std=gnu++11 -O3 -pthread -g3 -Wall -c -fmessage-length=0 -MMD


all:
# Compile classes
	g++ $(CPPFLAGS) -MF"prngs/GlibcRand.d" -MT"prngs/GlibcRand.d" -o "prngs/GlibcRand.o" "./prngs/GlibcRand.cpp"
	g++ $(CPPFLAGS) -MF"prngs/Mt19937.d" -MT"prngs/Mt19937.d" -o "prngs/Mt19937.o" "./prngs/Mt19937.cpp"
	g++ $(CPPFLAGS) -MF"prngs/Ruby.d" -MT"prngs/Ruby.d" -o "prngs/Ruby.o" "./prngs/Ruby.cpp"
	g++ $(CPPFLAGS) -MF"PRNGFactory.d" -MT"PRNGFactory.d" -o "PRNGFactory.o" "./PRNGFactory.cpp"

# Make the binary
	g++ $(CPPFLAGS) -MF"untwister.d" -MT"untwister.d" -o "untwister.o" "./untwister.cpp"
	g++ -std=gnu++11 -O3 -pthread -o "untwister" ./prngs/GlibcRand.o ./prngs/Mt19937.o ./prngs/Ruby.o ./PRNGFactory.o ./untwister.o 

clean:
	rm -f ./prngs/*.o
	rm -f ./prngs/*.d
	rm -f untwister untwister.o untwister.d PRNGFactory.o PRNGFactory.d
