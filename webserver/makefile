all:FifoFD.o HTTPdeamon.o ListOfWorkersDir.o Map.o MaxMinCountRes.o NetworkFunctions.o PostingList.o ResultParser.o SearchRes.o Statistics.o Trie.o WC.o Worker.o main.o main
CC = g++
OUT = main
OBJS = FifoFD.o HTTPdeamon.o ListOfWorkersDir.o Map.o MaxMinCountRes.o NetworkFunctions.o PostingList.o ResultParser.o SearchRes.o Statistics.o Trie.o WC.o Worker.o main.o
RM = rm -f

FifoFD.o: FifoFD.cpp FifoFD.h -lpthread
	$(CC) -c -o FifoFD.o FifoFD.cpp
HTTPdeamon.o: HTTPdeamon.cpp HTTPdeamon.h -lpthread
	$(CC) -c -o HTTPdeamon.o HTTPdeamon.cpp
ListOfWorkersDir.o: ListOfWorkersDir.cpp ListOfWorkersDir.h -lpthread
	$(CC) -c -o ListOfWorkersDir.o ListOfWorkersDir.cpp
Map.o: Map.cpp Map.h -lpthread
	$(CC) -c -o Map.o Map.cpp
MaxMinCountRes.o: MaxMinCountRes.cpp MaxMinCountRes.h -lpthread
	$(CC) -c -o MaxMinCountRes.o MaxMinCountRes.cpp
NetworkFunctions.o: NetworkFunctions.cpp  NetworkFunctions.h -lpthread
	$(CC) -c -o NetworkFunctions.o NetworkFunctions.cpp
PostingList.o: PostingList.cpp PostingList.h -lpthread
	$(CC) -c -o PostingList.o PostingList.cpp
ResultParser.o: ResultParser.cpp ResultParser.h -lpthread
	$(CC) -c -o ResultParser.o ResultParser.cpp
SearchRes.o: SearchRes.cpp SearchRes.h -lpthread
	$(CC) -c -o SearchRes.o SearchRes.cpp
Statistics.o: Statistics.cpp Statistics.h -lpthread
	$(CC) -c -o Statistics.o Statistics.cpp
Trie.o: Trie.cpp Trie.h -lpthread
	$(CC) -c -o Trie.o Trie.cpp
Worker.o: Worker.cpp Worker.h -lpthread
	$(CC) -c -o Worker.o Worker.cpp
main.o: main.cpp -lpthread
	$(CC) -c -o main.o main.cpp
main: FifoFD.o HTTPdeamon.o ListOfWorkersDir.o Map.o MaxMinCountRes.o NetworkFunctions.o PostingList.o ResultParser.o SearchRes.o Statistics.o Trie.o WC.o Worker.o main.o
	$(CC) -o main FifoFD.o HTTPdeamon.o ListOfWorkersDir.o Map.o MaxMinCountRes.o NetworkFunctions.o PostingList.o ResultParser.o SearchRes.o Statistics.o Trie.o WC.o Worker.o main.o -lpthread
clean:
	$(RM) $(OBJS) main

