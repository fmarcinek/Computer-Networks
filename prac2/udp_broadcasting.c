// Filip Marcinek 282905
#include "udp_broadcasting.h"

int create_and_prepare_socket()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return SOCKET_FAILURE;
	}

    int broadcastPermission = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (void *)&broadcastPermission, sizeof(broadcastPermission)))
	{
		fprintf(stderr, "setsockopt error:%s\n", strerror(errno) );
        return SOCKET_FAILURE;
	}

    struct sockaddr_in server_address;
	bzero(&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(PORT_NUMBER);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind (sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "bind error: %s\n", strerror(errno));
		return SOCKET_FAILURE;
	}
    return sockfd;
}

int send_routing_table(int socketfd, int n, router_rec routing_table[], net_vec vector_of_dists[]) {
    for (int i = 0; i < n; ++i) {
        char broadcast_ip[20];
        get_broadcast_ip(vector_of_dists[i].net_ip, broadcast_ip, vector_of_dists[i].netmask);
        for (int j = 0; j < n; ++j) {
            if (send_record(socketfd, &routing_table[j], broadcast_ip) == EXIT_FAILURE) {
                set_inf(vector_of_dists[i].net_ip, vector_of_dists[i].netmask, routing_table);
            }
        }
    }
    return EXIT_SUCCESS;
}

int send_record(int socketfd, const router_rec *rec, char *receiver_ip) {
    struct sockaddr_in server_address;
	bzero(&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(PORT_NUMBER);
	inet_pton(AF_INET, receiver_ip, &server_address.sin_addr);

    u_int8_t message[9];
    inet_pton(AF_INET, rec->net_ip, message);
    message[4] = rec->netmask;
    unsigned int net_distance = htonl(rec->distance);
    memcpy(message+5, &net_distance, sizeof(unsigned int));
    if (sendto(socketfd, message, sizeof(message), 0, (struct sockaddr*) &server_address, sizeof(server_address)) != sizeof(message)) {
        fprintf(stderr, "sendto error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}