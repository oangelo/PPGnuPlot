# neste template de Makefile so muda a lista
# dos SOURCES e o nome do EXECUTABLE.

CC = g++
CFLAGS = -c -Wall  -std=c++11 
LDFLAGS = -lgtest -lgsl -lgslcblas -lm -lpthread
SOURCES =  src/main.cpp  src/ppgnuplot.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = ppgnuplot 


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

