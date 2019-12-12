DESTDIR = D:/MinGW-W64/x86_64-win32-seh/mingw64/bin

all: justmake.exe

justmake.exe: main.o justmake.o directory.o
	g++ *.o -O3 -o justmake.exe

main.o: main.cpp justmake.h
	g++ -std=gnu++11 -O3 -c main.cpp

justmake.o: justmake.cpp justmake.h
	g++ -std=gnu++11 -O3 -c justmake.cpp

directory.o: directory.cpp directory.h
	g++ -std=gnu++11 -O3 -c directory.cpp

clean:
ifdef WINDOWS
	def /S /Q /F *.o justmake.exe
else
	rm -f *.o justmake.exe
endif

clear:
ifdef WINDOWS
	def /S /Q /F *.o
else
	rm -f *.o
endif

$(DESTDIR)/justmake.exe: justmake.exe
	cp justmake.exe $(DESTDIR)

install: $(DESTDIR)/justmake.exe

uninstall:
ifdef WINDOWS
	def /S /Q /F $(DESTDIR)/justmake.exe
else
	rm -f $(DESTDIR)/justmake.exe
endif