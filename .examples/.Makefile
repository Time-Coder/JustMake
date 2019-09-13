CC = g++
FLAGS = -std=c++11
LINK_FLAGS = -std=c++11
BINDIR = .bin

EXEDIR = .

EXTERN_INCLUDE = \

EXTERN_LIBPATH = \

EXTERN_LIBS = \

INCLUDE = \
-I./add/include \
-I./sub/include \

LIBPATH = \
-L./add/src \
-L./sub/lib \

LIBS = \
-ladd \
-lsub \

OBJS = \

DEPENDS_main = \
./main.cpp \
./sub/include/sub.h \

DEPENDS_main1 = \
./main1.cpp \
./add/include/add.h \
./sub/include/sub.h \

PATH := ./add/src;./sub/lib;$(PATH)

all: $(EXEDIR)/main.exe $(EXEDIR)/main1.exe 

$(EXEDIR)/main.exe: $(BINDIR)/main.o $(OBJS)
	$(CC) $(LINK_FLAGS) -Wl,-rpath=./add/src;./sub/lib; $(BINDIR)/main.o $(OBJS) -o $(EXEDIR)/main.exe $(LIBPATH) $(EXTERN_LIBPATH) $(LIBS) $(EXTERN_LIBS)

$(EXEDIR)/main1.exe: $(BINDIR)/main1.o $(OBJS)
	$(CC) $(LINK_FLAGS) -Wl,-rpath=./add/src;./sub/lib; $(BINDIR)/main1.o $(OBJS) -o $(EXEDIR)/main1.exe $(LIBPATH) $(EXTERN_LIBPATH) $(LIBS) $(EXTERN_LIBS)

$(BINDIR)/main.o: $(DEPENDS_main)
	$(CC) $(FLAGS) $(INCLUDE) $(EXTERN_INCLUDE) -c ./main.cpp -o $(BINDIR)/main.o

$(BINDIR)/main1.o: $(DEPENDS_main1)
	$(CC) $(FLAGS) $(INCLUDE) $(EXTERN_INCLUDE) -c ./main1.cpp -o $(BINDIR)/main1.o

run: $(EXEDIR)/main.exe
	$(EXEDIR)/main.exe

run_main: $(EXEDIR)/main.exe
	$(EXEDIR)/main.exe

run_main1: $(EXEDIR)/main1.exe
	$(EXEDIR)/main1.exe

clean:
	rm -f $(BINDIR)/*.o
	rm -f $(EXEDIR)/main.exe
	rm -f $(EXEDIR)/main1.exe

clear:
	rm -f $(BINDIR)/*.o
