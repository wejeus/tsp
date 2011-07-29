SRC=tsp.c
OBJ=${SRC:.c=.o}
CFLAGS+=-Wall -pedantic -std=c99 -g
CC=gcc

all: ${OBJ}
	${CC} -o tsp ${OBJ} -lm

%.o: $*.c
	${CC} ${CFLAGS} -c $*.c

clean:
	rm -f *.o tsp

