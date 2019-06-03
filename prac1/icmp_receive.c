// Filip Marcinek 282905

#include "icmp_receive.h"

#define IP_PACKAGE_LENGTH 8

int icmp_receive(int sockfd, int ttl, received_info *r_info)
{
	struct sockaddr_in 	sender;	
	socklen_t 			sender_len = sizeof(sender);
	u_int8_t 			buffer[IP_MAXPACKET];

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	while (1) {
		fd_set descriptors;
		FD_ZERO (&descriptors);
		FD_SET (sockfd, &descriptors);

		int ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);
		switch (ready) {
			case -1:
				fprintf(stderr, "select error: %s\n", strerror(errno));
				return EXIT_FAILURE;
			case 0:
				return EXIT_SUCCESS;
		}
		ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);
		if (packet_len < 0) {
			fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
			return EXIT_FAILURE;
		}

		struct timeval delivery_time;
		gettimeofday(&delivery_time, NULL);

		char sender_ip_str[20];
		inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));

		struct iphdr* 	ip_header = (struct iphdr*) buffer;
		ssize_t		  	ip_header_len = 4 * ip_header->ihl;

		u_int8_t* icmp_packet = buffer + ip_header_len;
		struct icmphdr* icmp_header = (struct icmphdr*) icmp_packet;
		
		if (icmp_header->type == ICMP_TIME_EXCEEDED) {
			u_int8_t* new_buffer = icmp_packet + IP_PACKAGE_LENGTH;
  			ip_header = (struct iphdr*) new_buffer;
			new_buffer += ip_header->ihl * 4;
			icmp_header = (struct icmphdr*) new_buffer;
		} 
		else if (icmp_header->type != ICMP_ECHOREPLY) {
			fprintf(stderr, "weird type of icmp massage: %u\n", icmp_header->type);
		}
		
		int id = icmp_header->un.echo.id;
		int seq = icmp_header->un.echo.sequence;

		if (id == session_pid && seq / ttl == NUM_OF_PACKAGES) {
			r_info->returned_packages++;
			r_info->received[seq % NUM_OF_PACKAGES] = 1;
			strcpy(r_info->ips[seq % NUM_OF_PACKAGES], sender_ip_str);
			memcpy((void*) &(r_info->stop[seq % NUM_OF_PACKAGES]), 
				   (void*) &delivery_time, sizeof(delivery_time));
		}
	}
	return EXIT_SUCCESS;
}
