TARGET = kmeans

CC = gcc
CFLAGS = -std=c99 -Wall -g

INCLUDE = "include"

SOURCE = src/main.c src/utils.c

LIBS = -lm

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -I $(INCLUDE) $(LIBS) -o $@ $^

teste: src/teste.c
	$(CC) $(CFLAGS) -o $@ $^
