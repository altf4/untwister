all:
	g++ -std=gnu++11 -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"prngs/GlibcRand.d" -MT"prngs/GlibcRand.d" -o "prngs/GlibcRand.o" "./prngs/GlibcRand.cpp"
	g++ -std=gnu++11 -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"prngs/Mt19937.d" -MT"prngs/Mt19937.d" -o "prngs/Mt19937.o" "./prngs/Mt19937.cpp"
	g++ -std=gnu++11 -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"PRNGFactory.d" -MT"PRNGFactory.d" -o "PRNGFactory.o" "./PRNGFactory.cpp"
	g++ -std=gnu++11 -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"untwister.d" -MT"untwister.d" -o "untwister.o" "./untwister.cpp"
	g++ -std=gnu++11 -O3 -g -o "untwister" ./prngs/GlibcRand.o ./prngs/Mt19937.o ./PRNGFactory.o ./untwister.o 

clean:
	rm -f ./prngs/GlibcRand.o ./prngs/Mt19937.o ./PRNGFactory.o ./untwister.o   ./prngs/GlibcRand.d ./prngs/Mt19937.d ./PRNGFactory.d ./untwister.d
	rm -f untwister
