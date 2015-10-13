TARGET = kmeans

CC = gcc
CFLAGS = -std=gnu99 -Wall -O3 -ggdb3

INCLUDE = "include"

SOURCE = src/main.c src/utils.c

LIBS = -lm

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -I $(INCLUDE) $(LIBS) -o $@ $^

clean:
	rm $(TARGET)

teste: src/teste.c src/utils.c
	$(CC) $(CFLAGS) -I $(INCLUDE) $(LIBS) -o $@ $^
