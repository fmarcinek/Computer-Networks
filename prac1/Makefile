# Filip Marcinek 282905

CC = gcc -ggdb 
CFLAGS = -Og -std=gnu99 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function

all: checksum send receive traceroute

checksum: icmp_checksum.o
send: icmp_send.o
receive: icmp_receive.o
traceroute: traceroute.o icmp_checksum.o icmp_send.o icmp_receive.o

clean:
	rm -f *.o
distclean:
	rm -f *.o
	rm -f traceroute
