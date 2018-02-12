CFLAGS=-g -Wall
CC=gcc
EXEC=diffckpt

all: $(EXEC) cleanup

$(EXEC): main.c mprotect-diffckpt.o
	$(CC) $(CFLAGS) -o $@ $^

mprotect-diffckpt.o: mprotect-diffckpt.c mprotect-diffckpt.h
	$(CC) $(CFLAGS) -c $<

cleanup:
	rm -rf *.o

run: $(EXEC) cleanup
	./$(EXEC)

clean:
	rm -rf ./$(EXEC)

.PHONY: cleanup clean run
