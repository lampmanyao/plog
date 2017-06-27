inc = -I../src/

cflags = -O3 -Wall

lib = -lpthread

VPATH = ./: ../src/

srcs = $(wildcard *.c)
srcs += $(wildcard ../src/*.c)
objs = $(srcs:.c=.o)

target = bench

all: $(target)

bench: $(objs)
	gcc $(cflags) -o $@ $^ $(inc) $(lib)

%.o: %.c
	gcc $(cflags) -c $^ -o $@ $(inc)

clean:
	rm -f *.o ../src/*.o
	rm -f $(target)
	rm -f *.log
