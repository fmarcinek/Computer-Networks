// Filip Marcinek 282905

#include "sending.h"
#include "header_info.h"

// Wysyla n bajtow z bufora buffer az do skutku.
void send_all (int sockfd, char* buffer, size_t n) {
	size_t nleft = n;
	while (nleft > 0) {
		int bytes_sent = send(sockfd, buffer, nleft, 0);
		if (bytes_sent < 0)
			ERROR("send error");
		fprintf(stderr, "DEBUG (Send): %d bytes sent\n", bytes_sent);
		nleft -= bytes_sent;
		buffer += bytes_sent;
	} 
}

void send_HTTP_response(HeaderInfo* hi, char* status, char* error_message, int sockfd, int filefd, char* ext) {
	if (error_message) {
		char buffer[1024];
		memset(buffer,0,sizeof(buffer));
		strcat(buffer, "HTTP/1.1 ");
		strcat(buffer, status);
		strcat(buffer, "\r\nContent-Type: html\r\nContent-Length: ");
		char message_size[10];
		sprintf(message_size, "%lu",strlen(error_message)+13);
		strcat(buffer, message_size);
		strcat(buffer, "\r\n\r\n<html>");
		strcat(buffer, error_message);
		strcat(buffer, "</html>");
		
		send_all(sockfd, buffer, strlen(buffer));
	}
	else {
		char buffer[512];
		memset(buffer,0,sizeof(buffer));
		strcat(buffer, "HTTP/1.1 ");
		strcat(buffer, status);
		strcat(buffer, "\r\nContent-Type: ");
        strcat(buffer, ext);
        strcat(buffer, "\r\nContent-Length: ");
        struct stat file_stat;
        if (fstat(filefd, &file_stat) < 0) {
            ERROR("fstat error");
        }
		char file_size[100];
		sprintf(file_size, "%ld",file_stat.st_size);
        strcat(buffer, file_size);
        strcat(buffer, "\r\n\r\n");
	
		send_all(sockfd, buffer, strlen(buffer));
        long int offset = 0;
        int remaining_bytes = file_stat.st_size;
        int sent_bytes = 0;
        while ((sent_bytes = sendfile(sockfd, filefd, &offset, remaining_bytes)) > 0 && remaining_bytes > 0) {
            remaining_bytes -= sent_bytes;
            fprintf(stderr, "DEBUG: Sending %d bytes of file\n",sent_bytes);
        }
        if (remaining_bytes > 0) {
            ERROR("Error occured during sending a file to socket");
        }
	}
}
