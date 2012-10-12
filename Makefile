# neste template de Makefile so muda a lista
# dos SOURCES e o nome do EXECUTABLE.

CC=g++
CFLAGS=-c -Wall -std=c++0x  
LDFLAGS=-lgtest
SOURCES= unit_test/tests.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=tests


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

