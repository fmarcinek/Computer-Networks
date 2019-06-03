# Filip Marcinek 282905

CC = gcc -ggdb 
CFLAGS = -Og -std=gnu99 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function

all: receive header send webserver

receive: receiving.o
header: header_info.o
send: sending.o
webserver: webserver.o receiving.o header_info.o sending.o

clean:
	rm -f *.o
distclean:
	rm -f *.o
	rm -f webserver
