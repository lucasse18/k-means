CC   = gcc
INCDIR  = include

CFLAGS = -std=gnu99 -I ${INCDIR} -lm

RELFLAGS = ${CFLAGS} -DNDEBUG -O2
DBGFLAGS = ${CFLAGS} -Wall -ggdb3

SOURCE = src/main.c\
	src/vector.c\
	src/dataset.c\
	src/kmeans.c

kmeans:${SOURCE}
	${CC} ${RELFLAGS} -o $@ $^

dbg:${SOURCE}
	${CC} ${DBGFLAGS} -o kmeans-$@ $^
