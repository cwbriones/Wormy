CC=g++
CFLAGS=-c -Wall -O2
LDFLAGS=-lncurses
SOURCES=main.cpp worm.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=worm

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o
