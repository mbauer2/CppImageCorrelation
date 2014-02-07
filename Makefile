# Makefile for assn7

assn7:	main.o Reader.o
	g++ -O2 -Wall main.o Reader.o -o assn7

Reader.o: Reader.h Reader.cpp
	g++ -O2 -Wall -g -I. Reader.cpp -c -o Reader.o

main.o:	main.cpp
	g++ -O2 -Wall -g -I. main.cpp -c -o main.o

clean:	
	rm -f *.o
	rm -f assn7

