// Filip Marcinek 282905

#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "icmp_send.h"
#include "icmp_checksum.h"

int icmp_send(int sockfd, int ttl, int seq_num, char *target_addr) {
    struct icmphdr icmp_header;
    icmp_header.type = ICMP_ECHO;
    icmp_header.code = 0;
    icmp_header.un.echo.id = session_pid;
    icmp_header.un.echo.sequence = seq_num;
    icmp_header.checksum = compute_icmp_checksum((u_int16_t*)&icmp_header, sizeof(icmp_header));

    // struktura odbierająca
    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    inet_pton(AF_INET, target_addr, &recipient.sin_addr);

    // ustawienie ttl
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

    // wysłanie pakietu
    ssize_t bytes_sent = sendto(sockfd, &icmp_header, sizeof(icmp_header), 
                                0, (struct sockaddr*)&recipient, sizeof(recipient));
    if (bytes_sent < 0) {
        fprintf(stderr, "sendto error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}