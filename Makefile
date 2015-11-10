TARGET = kmeans

CC = gcc
CFLAGS = -std=gnu99 -Wall -O3 -ggdb3

INCLUDE = "include"

SOURCE = src/main.c src/utils.c

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -I $(INCLUDE) -o $@ $^

clean:
	rm $(TARGET)
