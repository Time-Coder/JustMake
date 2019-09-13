CC = g++
FLAGS = -std=c++11
LINK_FLAGS = -std=c++11
BINDIR = .

EXTERN_INCLUDE = \

EXTERN_LIBPATH = \

EXTERN_LIBS = \

INCLUDE = \
-I. \

LIBPATH = \

LIBS = \

OBJS = \

DEPENDS_main1 = \
./main1.cpp \

DEPENDS_main = \
./main.cpp \

all: $(BINDIR)/main1 $(BINDIR)/main 

$(BINDIR)/main1: $(BINDIR)/main1.o $(OBJS)
	$(CC) $(LINK_FLAGS) $(BINDIR)/main1.o $(OBJS) -o $(BINDIR)/main1 $(LIBPATH) $(EXTERN_LIBPATH) $(LIBS) $(EXTERN_LIBS)

$(BINDIR)/main: $(BINDIR)/main.o $(OBJS)
	$(CC) $(LINK_FLAGS) $(BINDIR)/main.o $(OBJS) -o $(BINDIR)/main $(LIBPATH) $(EXTERN_LIBPATH) $(LIBS) $(EXTERN_LIBS)

$(BINDIR)/main1.o: $(DEPENDS_main1)
	$(CC) $(FLAGS) $(INCLUDE) $(EXTERN_INCLUDE) -c ./main1.cpp -o $(BINDIR)/main1.o

$(BINDIR)/main.o: $(DEPENDS_main)
	$(CC) $(FLAGS) $(INCLUDE) $(EXTERN_INCLUDE) -c ./main.cpp -o $(BINDIR)/main.o

run: $(BINDIR)/main1
	$(BINDIR)/main1

run_main1: $(BINDIR)/main1
	$(BINDIR)/main1

run_main: $(BINDIR)/main
	$(BINDIR)/main

clean:
	rm -f $(BINDIR)/*.o
	rm -f $(BINDIR)/main1
	rm -f $(BINDIR)/main

clear:
	rm -f $(BINDIR)/*.o
