TARGET = kmeans

CC = gcc
CFLAGS = -std=c99 -Wall -Werror -g

SOURCE = main.c utils.c

LIBS = -lm

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

