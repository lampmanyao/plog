inc = -I../src/

cflags = -O3 -Wall

lib = -lpthread

VPATH = ./: ../src/

target = test_plog

all: $(target)


test_plog: test_plog.o condition.o plog.o
	gcc $(cflags) -o $@ $^ $(inc) $(lib)

test_plog.o: test_plog.c
	gcc $(cflags) -c $^ $(inc)

condition.o: condition.c
	gcc $(cflags) -c $^ $(inc)

plog.o: plog.c
	gcc $(cflags) -c $^ $(inc)

clean:
	rm -f *.o
	rm -f $(target)
	rm -f *.log
