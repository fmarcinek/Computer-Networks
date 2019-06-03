# Filip Marcinek 282905

CC = gcc -ggdb 
CFLAGS = -Og -std=gnu99 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function

all: broadcast receive router

broadcast: udp_broadcasting.o
receive: udp_receiving.o
router: router.o udp_broadcasting.o udp_receiving.o

clean:
	rm -f *.o
distclean:
	rm -f *.o
	rm -f router
