// Filip Marcinek 282905
#ifndef UDP_BROADCASTING_H
#define UDP_BROADCASTING_H

#include "common_includes.h"

int create_and_prepare_socket();
int send_routing_table(int socketfd, int n, router_rec routing_table[], net_vec vector_of_dists[]);
int send_record(int socketfd, const router_rec *rec, char *receiver_ip);

#endif