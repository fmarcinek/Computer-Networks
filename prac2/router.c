// Filip Marcinek 282905

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "udp_receiving.h"
#include "udp_broadcasting.h"

void show_routing_table(int n, router_rec routing_table[]) {
    for(int i = 0; i < n; ++i) {
        printf("%s/%u",routing_table[i].net_ip,routing_table[i].netmask);
        if(routing_table[i].distance < INF) {
            printf(" distance %u",routing_table[i].distance);
        } else {
            printf(" unreachable");
        }
        if (routing_table[i].neighbour) {
            printf(" connected directly\n");
        } else {
            printf(" via %s\n",routing_table[i].via);
        }
    }
    printf("\n");
}

int main() {
    int n;
    scanf("%d\n",&n);
    char ip[20];
    char net_size;
    unsigned int dist;
    for (int i = 0; i < n; ++i) {
        scanf("%[^/]", ip);
        scanf("/%c",&net_size);
        scanf(" distance %u\n",&dist);
    }

    router_rec *routing_table = (router_rec *) malloc(n * sizeof(router_rec));
    net_vec *vector_of_dists = (net_vec *) malloc(n * sizeof(net_vec));

    int socketfd = create_and_prepare_socket();
    if (socketfd == -1) {
        fprintf(stderr, "create_and_prepare_socket error:%s\n", strerror(errno) );
        return EXIT_FAILURE;
    }

    struct timeval my_time;
    my_time.tv_sec = ROUND_TIME / SHOWS_PER_ROUND;
    my_time.tv_usec = 0;

    while(TRUE) {
        fd_set descriptors;
        FD_ZERO(&descriptors);
        FD_SET(socketfd, &descriptors);
        int ready = select(socketfd+1, &descriptors, NULL, NULL, &my_time);
        if (ready < 0) {
            fprintf(stderr, "select error: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        if (ready == 0) {
            show_routing_table(n, routing_table);
            increase_freezed_counter(n, routing_table);
            mark_unactive_nets(n, routing_table);
            send_routing_table(socketfd, n, routing_table, vector_of_dists);
        } else {
            router_rec temp;
            bzero(&temp, sizeof(temp));
            receive_packet(socketfd, &temp);
            update_vector(n, routing_table, &temp);
        }
    }
    close(socketfd);
    return EXIT_SUCCESS;
}