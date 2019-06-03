// Filip Marcinek 282905

#ifndef SEND_H
#define SEND_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

extern int session_pid;

int icmp_send(int sockfd, int ttl, int seq_num, char *target_addr);

#endif