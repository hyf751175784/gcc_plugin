main: main.o Options.o
	g++ -std=c++11 main.o Options.o -o main

main.o: main.cpp
	g++ -std=c++11 main.cpp -c -o main.o

Options.o: Options.cpp
	g++ -std=c++11 Options.cpp -c -o Options.o

Options.o :Options.h

clean:
	rm *.o main -rf
