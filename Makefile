CC = gcc
CFLAGS = -std=gnu99 -Wall -ggdb3 -lm

SOURCE = src/main.c\
	src/vector.c\
  src/dataset.c\
	src/kmeans.c

kmeans:${SOURCE}
	${CC} ${CFLAGS} -o $@ $^
