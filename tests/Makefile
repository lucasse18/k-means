CC   = gcc
STD  = -std=gnu99
DBG  = -ggdb3 -Wall
INC  = ../include

CFLAGS = ${STD} ${DBG} -I ${INC}

SOURCE = src/process.c

process-output:${SOURCE}
	${CC} ${CFLAGS} -DNDEBUG -o $@ $^
