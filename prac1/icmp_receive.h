// Filip Marcinek 282905

#ifndef RECEIVE_H
#define RECEIVE_H

#define NUM_OF_PACKAGES 3

extern int session_pid;

typedef struct {
    char ips[NUM_OF_PACKAGES][20];
    long times[NUM_OF_PACKAGES];
    char received[NUM_OF_PACKAGES];
    int returned_packages;
    struct timeval start[NUM_OF_PACKAGES];
    struct timeval stop[NUM_OF_PACKAGES];
} received_info;

int icmp_receive(int sockfd, int ttl, received_info *r_info);

#endif