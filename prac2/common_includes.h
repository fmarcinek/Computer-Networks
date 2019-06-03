#ifndef COMMON_INCLUDES_H
#define COMMON_INCLUDES_H

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#define TRUE  1
#define FALSE 0

#define INF             0xffffffff // == 2**32-1
#define ROUND_TIME      30
#define LIMIT           2
#define PORT_NUMBER     54321
#define SOCKET_FAILURE  -1
#define SHOWS_PER_ROUND 3

typedef struct {
    char net_ip[20];
    char via[20];
    char netmask;
    unsigned int distance;
    char reachable;
    char rounds_unreachable;
    char neighbour;
    char freezed_distance;
    char rounds_freezed;
} router_rec;

typedef struct {
    char ip[20];
    char net_ip[20];
    char netmask;
    unsigned int distance;
} net_vec;

#endif