.phony all:
all: rsi

rsi: rsi.c
	gcc pman.c -lreadline -o pman

.PHONY clean:
clean:
	-rm -rf *.o *.exe
