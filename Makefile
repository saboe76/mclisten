PREFIX=/usr/bin
CFLAGS = -O2 -Wall
BIN = mclisten

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

all:	$(BIN)

mclisten:	mclisten.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(BIN) *.o

.PHONY: all clean mclisten


