// Filip Marcinek 282905

#ifndef SENDING_H
#define SENDING_H

#include "common_includes.h"

void send_all (int sockfd, char* buffer, size_t n);
void send_HTTP_response(HeaderInfo* hi, char* status, char* error_message, int sockfd, int filefd, char* ext);

#endif