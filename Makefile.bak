
CFLAGS += -Wall -pedantic -std=c99 -g -lm
#SOURCES = 
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = tsp
CC = gcc

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) tsp.c
	${CC} $(CFLAGS) $(OBJECTS) tsp.c -o $(EXECUTABLE)

.c.o:
	${CC} $(CFLAGS) -c $< -o $@

2opt: $(OBJECTS)
	${CC} $(CFLAGS) -o tsp 2opt.c -lm


report: report.tex
	pdflatex report.tex
	pdflatex report.tex

.PHONY: clean
clean:
	rm -fr *.o tsp *dSYM
	rm -f report.aux report.log report.synctex.gz
	

