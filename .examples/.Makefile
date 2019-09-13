CC = g++
FLAGS = -std=c++11
LINK_FLAGS = -std=c++11
BINDIR = .bin

EXEDIR = .

EXTERN_INCLUDE = \

EXTERN_LIBPATH = \

EXTERN_LIBS = \

INCLUDE = \
-I./sub/include \
-I./add/include \

LIBPATH = \

LIBS = \

OBJS = \
$(BINDIR)/sub.o \
$(BINDIR)/add.o \

DEPENDS_main1 = \
./main/main1.cpp \
./add/include/add.h \
./sub/include/sub.h \

DEPENDS_main = \
./main/main.cpp \
./sub/include/sub.h \

DEPENDS_sub = \
./sub/src/sub.cpp \
./sub/include/sub.h \

DEPENDS_add = \
./add/src/add.cpp \
./add/include/add.h \

all: $(EXEDIR)/main1.exe $(EXEDIR)/main.exe 

$(EXEDIR)/main1.exe: $(BINDIR)/main1.o $(OBJS)
	$(CC) $(LINK_FLAGS) $(BINDIR)/main1.o $(OBJS) -o $(EXEDIR)/main1.exe $(LIBPATH) $(EXTERN_LIBPATH) $(LIBS) $(EXTERN_LIBS)

$(EXEDIR)/main.exe: $(BINDIR)/main.o $(OBJS)
	$(CC) $(LINK_FLAGS) $(BINDIR)/main.o $(OBJS) -o $(EXEDIR)/main.exe $(LIBPATH) $(EXTERN_LIBPATH) $(LIBS) $(EXTERN_LIBS)

$(BINDIR)/main1.o: $(DEPENDS_main1)
	$(CC) $(FLAGS) $(INCLUDE) $(EXTERN_INCLUDE) -c ./main/main1.cpp -o $(BINDIR)/main1.o

$(BINDIR)/main.o: $(DEPENDS_main)
	$(CC) $(FLAGS) $(INCLUDE) $(EXTERN_INCLUDE) -c ./main/main.cpp -o $(BINDIR)/main.o

$(BINDIR)/sub.o: $(DEPENDS_sub)
	$(CC) $(FLAGS) $(INCLUDE) $(EXTERN_INCLUDE) -c ./sub/src/sub.cpp -o $(BINDIR)/sub.o

$(BINDIR)/add.o: $(DEPENDS_add)
	$(CC) $(FLAGS) $(INCLUDE) $(EXTERN_INCLUDE) -c ./add/src/add.cpp -o $(BINDIR)/add.o

run: $(EXEDIR)/main1.exe
	$(EXEDIR)/main1.exe

run_main1: $(EXEDIR)/main1.exe
	$(EXEDIR)/main1.exe

run_main: $(EXEDIR)/main.exe
	$(EXEDIR)/main.exe

clean:
	rm -f $(BINDIR)/*.o
	rm -f $(EXEDIR)/main1.exe
	rm -f $(EXEDIR)/main.exe

clear:
	rm -f $(BINDIR)/*.o
