// Filip Marcinek 282905

#include "udp_receiving.h"

int receive_packet(int socketfd, router_rec *rec)
{
    struct sockaddr_in 	sender;
	socklen_t 			sender_len = sizeof(sender);
	u_int8_t 			buffer[IP_MAXPACKET+1];
    ssize_t datagram_len = recvfrom(socketfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);
	if (datagram_len < 0)
    {
        fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
        return EXIT_FAILURE;
	}
    inet_ntop(AF_INET, &(sender.sin_addr), rec->via, 20 * sizeof(char));
    inet_ntop(AF_INET, buffer, rec->net_ip, 20 * sizeof(char) );
    rec->netmask = buffer[4];

    unsigned int net_distance;
    memcpy(&net_distance, buffer+5, sizeof(net_distance));
    rec->distance = ntohl(net_distance);
    return EXIT_SUCCESS;
}

unsigned int gen_mask(char m) { 
    return (1 << (32-m)) - 1;
}

void get_ip(const char *src, char *dst, char mask)
{
    struct in_addr temp;
    inet_pton(AF_INET, src, &temp);
    unsigned int addr = ntohl( temp.s_addr);
    addr = addr & ~gen_mask(mask);
    addr = htonl(addr);
    inet_ntop(AF_INET, &addr, dst, 20);
}

void set_inf(char *net_ip, char mask, int n, router_rec routing_table[]) {
    for(int i = 0; i < n; i++)
    {
        char from_net[20];
        get_ip(routing_table[i].via, from_net, mask);
        if (strcmp(routing_table[i].net_ip, net_ip) == 0 || strcmp(from_net,net_ip) == 0)
        {
            routing_table[i].freezed_distance = TRUE;
            routing_table[i].distance = INF;
        }
    }
}

int find_neighbour_rec(char *ip, int n, net_vec vector_of_dists[], net_vec *result)
{
    char net_ip[20];
    for(size_t i = 0; i < n; i++) {
        get_ip(ip, net_ip, vector_of_dists[i].netmask);
        if (strcmp(vector_of_dists[i].net_ip, net_ip) == 0 && strcmp(ip,vector_of_dists[i].ip) != 0)
        {
            *result = vector_of_dists[i];
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}



int find_routing_rec(char * ip, char netmask, int n, router_rec vec[], router_rec ** result)
{
    char net_ip[20];
    get_ip(ip, net_ip, netmask);
    for (int i = 0; i < n; ++i)
    {
        if( strcmp(vec[i].net_ip  ,net_ip) == 0 )
        {
            *result = vec+i;
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}


char is_neighbour(router_rec *rec)
{
    return strlen(rec->via) == 0;
}

void update_vector(int n, router_rec vec[], router_rec *packet)
{
    net_vec neib_rec;
    bzero(&neib_rec, sizeof(net_vec) );
    if (find_neighbour_rec(packet->via, n, vec, &neib_rec)) return;
    mark_reached(packet->via, n, vec);

    unsigned int new_distance = packet->distance >= INF ? packet->distance : packet->distance + neib_rec.distance;

    router_rec *r_rec;
    if (find_routing_rec(packet->net_ip, packet->netmask, n, vec, &r_rec)) {
        if (new_distance >= INF) return;
        packet->distance = new_distance;
        *r_rec = *packet;
        return;
    }
    if (r_rec->freezed_distance) return;

    if (new_distance == INF) {
        if (strcmp(r_rec->via, packet->via) == 0) {
            r_rec->freezed_distance = TRUE;
            r_rec->rounds_freezed = 0;
            r_rec->distance = INF;
        }
    }
    if (new_distance < r_rec->distance) {
        r_rec->distance = new_distance;
        strcpy(r_rec->via, packet->via);
    }
}

void increase_freezed_counter(int n, router_rec routing_table[]) {
    for (int i = 0; i < n; ++i) {
        if (routing_table[i].freezed_distance) {
            if (routing_table[i].rounds_freezed >= LIMIT) {
                if (! routing_table[i].neighbour) {
                    vector_delete(n, routing_table, i);
                    --i;
                } else {
                    routing_table[i].freezed_distance = FALSE;
                }
            } else {
                routing_table[i].rounds_freezed++;
            }
        }
    }
}

void vector_delete(int n, router_rec r_table[], int i)
{
    for(int j = i; j < n-1; ++j) {
        r_table[i] = r_table[j];
    }
    --n;
}

void copy_neighbour_rec(const net_vec *src, router_rec *dst)
{
    bzero(dst,sizeof(router_rec));
    strcpy(dst->net_ip, src->net_ip);
    dst->netmask = src->netmask;
    dst->distance = src->distance;
    dst->reachable = TRUE;
    dst->neighbour = TRUE;
}

int mark_reached(char *ip, int n, net_vec vector_of_dists[], router_rec vec[]) {
  for (int i = 0; i < n; ++i) {
    if (vec[i].neighbour) {
        char net_ip[20];
        get_ip(ip, net_ip, vec[i].netmask);
        if(strcmp(net_ip, vec[i].net_ip) == 0) {
            if(!vec[i].reachable) {
                vec[i].reachable= TRUE;
                vec[i].rounds_unreachable = 0;
                vec[i].freezed_distance = FALSE;
                vec[i].rounds_freezed = 0;
                net_vec r;
                find_neighbour_rec(net_ip, n, vector_of_dists, &r);
                vec[i].distance = r.distance;
                return 1;
            }
        }
    }
  }
  return EXIT_SUCCESS;
}

void mark_unactive_nets(int n, router_rec vec[])
{
    for (int i = 0; i < n; ++i) {
        if(vec[i].neighbour && vec[i].rounds_freezed <= LIMIT) {
            if(vec[i].reachable) {
                vec[i].reachable = FALSE;
            } else {
                if (vec[i].rounds_unreachable <= LIMIT) {
                    vec[i].rounds_unreachable++;
                } else {
                    vec[i].distance = INF;
                    vec[i].freezed_distance = TRUE;
                    set_inf(vec[i].net_ip,vec[i].netmask,n,vec);
                }
            }
        }
  }
}

void get_broadcast_ip(const char *src, char *dst, char mask) {
    struct in_addr temp;
    inet_pton(AF_INET, src, &temp);
    unsigned int addr = ntohl( temp.s_addr);
    addr = addr | gen_mask(mask);
    temp.s_addr = htonl(addr);
    inet_ntop(AF_INET, &temp.s_addr, dst, 20);
}