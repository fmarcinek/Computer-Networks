// Filip Marcinek 282905

#ifndef COMMON_INCLUDES_H
#define COMMON_INCLUDES_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#define ERROR(str) { fprintf(stderr, "%s: %s\n", str, strerror(errno)); exit(EXIT_FAILURE); }
#define MAX_PORT_NUMBER 65535

typedef struct {
	char url[50];
	char host[20];
	int port;
	bool connection; //  true gdy chcemy utrzymac polaczenie, false gdy chcemy zamknac
} HeaderInfo;

#endif