TARGET = kmeans

CC = gcc
CFLAGS = -std=gnu99 -Wall -Og -ggdb3

INCLUDE = "include"

SOURCE = src/main.c src/utils.c

LIBS = -lm

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -I $(INCLUDE) $(LIBS) -o $@ $^

clean:
	rm $(TARGET)
