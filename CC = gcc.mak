CC = gcc

CFLAGS = -Wall -Werror -Wextra -std=c11 -pthread

OUT_DIR = out

TARGETS = $(OUT_DIR)/main

all: main.o
	$(CC) $(CFLAGS) $^ -lm -o $(OUT_DIR)/lab_2.out

main.o: main.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.out *.o

rebuild: clean all