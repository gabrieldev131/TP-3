# makefile

all: main

main: main.o exames.o
	gcc -g -o main main.o exames.o
	rm -rf *.o
main.o: main.c exames.h
	gcc -o main.o main.c -c -W -Wall -ansi -pedantic -std=gnu11

exames.o: exames.c exames.h
	gcc -o exames.o exames.c -c -W -Wall -ansi -pedantic -std=gnu11

clean:
	rm -rf *.o *~ main
