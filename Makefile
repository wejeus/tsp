
CFLAGS += -Wall -pedantic -std=c99 -g
#SOURCES = 
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = app
CC = gcc

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) main.cpp
	${CC} $(OBJECTS) main.cpp -o $(EXECUTABLE)

.c.o:
	${CC} $(CFLAGS) -c $< -o $@

2opt: $(OBJECTS)
	${CC} $(CFLAGS) -o tsp 2opt.c -lm


report: report.tex
	pdflatex report.tex
	pdflatex report.tex

.PHONY: clean
clean:
	rm -f *.o tsp *dSYM
	

