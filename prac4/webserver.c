// Filip Marcinek 282905

#include "common_includes.h"
#include "header_info.h"
#include "receiving.h"
#include "sending.h"

int argsHandling(int argc, char **argv, int *port, DIR* dir) {
	int i = 0;
	*port = 0;
	while (argv[1][i] != '\0') {
		if (isdigit(argv[1][i])) {
			*port *= 10;
			*port += argv[1][i] - '0';
			++i;
		}
		else {
			fprintf(stderr, "Port number is in bad format\n");
			return EXIT_FAILURE;
		}
	}
	if (*port > MAX_PORT_NUMBER) {
		fprintf(stderr, "Port number is too big\n");
		return EXIT_FAILURE;
	}

	dir = opendir(argv[2]);
	if (dir) {
		return EXIT_SUCCESS;
	}
	else if (errno == ENOENT) {
		ERROR("This directory does not exists (maybe path format is uncorrect)");
	}
	else {
		ERROR("Get correct path to existing directory");
	}
	return EXIT_FAILURE;
}

void close_connection(int sockfd) {
    if (close(sockfd) < 0)
        ERROR("close error");
    fprintf(stderr, "DEBUG: Disconnected\n");
}
	
int main(int argc, char** argv)
{
	if (argc < 3) {
		fprintf(stderr, "not enough arguments were given to the program\n");
		return EXIT_FAILURE;
	}
	DIR* dir = NULL;
	int port;
	int res = argsHandling(argc, argv, &port, dir);
	if (res == EXIT_FAILURE) {
		fprintf(stderr, "arguments given to the program are in bad format\n");
		return EXIT_FAILURE;
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if (sockfd < 0)
		ERROR("socket error");

	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(port);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind (sockfd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) 
		ERROR("bind error");

	if (listen(sockfd, 64) < 0)
		ERROR("listen error");

	while (true) {
		struct sockaddr_in client_address;
		socklen_t len = sizeof(client_address);
		int conn_sockfd = accept(sockfd, (struct sockaddr*) &client_address, &len);
		if (conn_sockfd < 0)
			ERROR("accept error")
		char ip_address[20];
		inet_ntop(AF_INET, &client_address.sin_addr, ip_address, sizeof(ip_address));
		fprintf(stderr, "DEBUG: New client %s:%d\n", ip_address, ntohs(client_address.sin_port));

		const int BUFFER_SIZE = 1000;
		char recv_buffer[BUFFER_SIZE+1];
        while (true)
        {
            // czekamy na dane
            int n = get_data(conn_sockfd, recv_buffer, BUFFER_SIZE);
            if (n == -2) {	   // dodatkowa sekunda minela
                fprintf (stderr, "DEBUG: Timeout\n");
                close_connection(conn_sockfd);
                break;
            } else if (n == -1) {
                fprintf (stderr, "DEBUG: Read error: %s\n", strerror(errno));
                close_connection(conn_sockfd);
                break;
            } else if (n == 0) {
                fprintf (stderr, "DEBUG: Client closed connection");
                close_connection(conn_sockfd);
                break;
            } else {
                recv_buffer[n] = 0;
                HeaderInfo *hi = extractHeaderInfo(recv_buffer);
                fprintf(stderr,"DEBUG: requested page: %s\n", hi->url);
                if (!hi) {
                    fprintf(stderr, "Unsupported format of HTTP Header\n");
                    send_HTTP_response(hi,"501 Not Implemented","Unsupported format of HTTP Header",conn_sockfd,0,NULL);
                }
                else {
                    char resolved_path[100];
                    get_realpath(hi, argv[2], resolved_path);
                    if (! target_file_is_in_domain(hi,argv[2],resolved_path)) {
                        fprintf(stderr, "Forbidden request (outer from domain)\n");
                        send_HTTP_response(hi,"403 Forbidden","Forbidden request (outer from domain)",conn_sockfd,0,NULL);
                    }
                    else {
                        if (is_dir(resolved_path)) {
                            char extended_path[120];
                            memset(extended_path,0,120);
                            strcat(extended_path,resolved_path);
                            int len = strlen(resolved_path);
                            if (resolved_path[len-1] != '/') {
                                extended_path[len] = '/';
                                extended_path[len+1] = '\0';
                            }
                            strcat(extended_path,"index.html");
                            int filefd = open(extended_path, O_RDONLY);
                            if (filefd == -1) {
                                fprintf(stderr, "File does not exists\n");
                                send_HTTP_response(hi,"404 Not Found","File index.html does not exists",conn_sockfd,0,NULL);
                            }
                            else {
                                char ext[30];
                                memset(ext,0,sizeof(ext));
                                get_extension(extended_path,ext);
                                fprintf(stderr, "Sending index.html from requested directory\n");
                                send_HTTP_response(hi,"301 Moved Permanently",NULL,conn_sockfd,filefd,ext);
                            }
                        }
                        else {
                            int filefd = open(resolved_path, O_RDONLY);
                            if (filefd == -1) {
                                fprintf(stderr, "File does not exists\n");
                                send_HTTP_response(hi,"404 Not Found","File index.html does not exists",conn_sockfd,0,NULL);
                            }
                            else {
                                char ext[30];
                                memset(ext,0,sizeof(ext));
                                get_extension(resolved_path,ext);
                                send_HTTP_response(hi,"200 OK",NULL,conn_sockfd,filefd,ext);
                            }
                        }
                    }
                }
                if (! hi->connection) {     // if Connection: close
                    close_connection(conn_sockfd);
                    break;
                }
            }
        }
	}
}