// Filip Marcinek 282905

#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <stdlib.h>

u_int16_t compute_icmp_checksum (const void *buff, int length);

#endif