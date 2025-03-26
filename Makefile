CC=g++
CFLAGS= -std=c++17 -g
LIBS= -lglut -lGLEW -lGL -lGLU
# Default target executed when no arguments are given to make.
default_target: final
.PHONY : default_target

final: final.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

final.o:
	$(CC) $(CFLAGS) -c final.cpp

clean:
	rm -f final *~ *.o

