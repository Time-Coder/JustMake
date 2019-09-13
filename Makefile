all: justmake.exe

justmake.exe: main.o justmake.o directory.o
	g++ *.o -o justmake.exe

main.o: main.cpp justmake.h
	g++ -std=c++11 -c main.cpp

justmake.o: justmake.cpp justmake.h
	g++ -std=c++11 -c justmake.cpp

directory.o: directory.cpp directory.h
	g++ -std=c++11 -c directory.cpp

clean:
	rm -f *.o
	rm -f justmake.exe
	rm -f JustMakefile