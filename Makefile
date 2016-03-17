CC = gcc
STD=-std=gnu99
DBG=-ggdb3 -Wall
LIBM=-lm
INC=include

CFLAGS = ${STD} ${DBG} ${LIBM} -I ${INC}

SOURCE = src/main.c\
	src/vector.c\
	src/dataset.c\
	src/kmeans.c

kmeans:${SOURCE}
	${CC} ${CFLAGS} -o $@ $^
