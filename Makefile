all: justmake

justmake: main.o justmake.o directory.o
	g++ *.o -o justmake

main.o: main.cpp justmake.h
	g++ -std=c++11 -c main.cpp

justmake.o: justmake.cpp justmake.h
	g++ -std=c++11 -c justmake.cpp

directory.o: directory.cpp directory.h
	g++ -std=c++11 -c directory.cpp

clean:
	rm -f *.o
	rm -f justmake
	rm -f JustMakefile

clear:
	rm -f *.o

install: justmake
	cp justmake /usr/bin