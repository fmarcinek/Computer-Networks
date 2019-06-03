// Filip Marcinek 282905

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "icmp_send.h"
#include "icmp_receive.h"

#define MIN_TTL 1
#define MAX_TTL 30

#define TRUE 1
#define FALSE 0

int session_pid;

int correct_ip(char *ip) {
    struct in_addr sin_addr;
    return inet_pton(AF_INET, ip, &sin_addr) == 1;
}

int miliseconds(struct timeval start, struct timeval stop) {
    return (stop.tv_sec - start.tv_sec)*1000
         + (stop.tv_usec - start.tv_usec)/1000;
}

void figure_and_print_results(received_info *r_info) {
    if (r_info->returned_packages == 0) {
        printf("*\n");
    }
    else {
        int avg_time = 0;
        for (int i = 0; i < NUM_OF_PACKAGES; ++i) {
            if (r_info->received[i]) {
                printf("%s ", r_info->ips[i]);
                avg_time += miliseconds(r_info->start[i], r_info->stop[i]);
            }
        }
        avg_time /= NUM_OF_PACKAGES;
        if (r_info->returned_packages == NUM_OF_PACKAGES) {
            printf("%dms\n",avg_time);
        }
        else printf("???\n");
    }
}

char target_achieved(const char *target_ip, received_info *r_info) {
    for (int i = 0; i < NUM_OF_PACKAGES; ++i) {
        if (r_info->received[i] && !strcmp(target_ip, r_info->ips[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Podaj IP\n");
        return EXIT_FAILURE;
    }
    if (! correct_ip(argv[1])) {
        fprintf(stderr, "Nieprawidłowy format IP\n");
        return EXIT_FAILURE;
    }

    session_pid = getpid();
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

    for (int ttl = MIN_TTL; ttl <= MAX_TTL; ++ttl) {
        received_info r_info;
        r_info.returned_packages = 0;
        for (int i = 0; i < NUM_OF_PACKAGES; ++i) r_info.received[i] = 0;

        for (int i = 0; i < NUM_OF_PACKAGES; ++i) {
            int seq_num = NUM_OF_PACKAGES*ttl+i;
            icmp_send(sockfd, ttl, seq_num, argv[1]);
            gettimeofday(&(r_info.start[i]),NULL);
        }

        icmp_receive(sockfd, ttl, &r_info);
        figure_and_print_results(&r_info);

        if (target_achieved(argv[1], &r_info)) {
            return EXIT_SUCCESS;
        }
    }
    return EXIT_SUCCESS;
}