all: automake.exe

automake.exe: main.o automake.o directory.o
	g++ *.o -o automake.exe

main.o: main.cpp automake.h
	g++ -std=c++11 -c main.cpp

automake.o: automake.cpp automake.h
	g++ -std=c++11 -c automake.cpp

directory.o: directory.cpp directory.h
	g++ -std=c++11 -c directory.cpp

clean:
	rm -f *.o
	rm -f automake.exe
	rm -f AutoMakefile