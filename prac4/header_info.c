// Filip Marcinek 282905

#include "header_info.h"
#include "common_includes.h"

void get_realpath(HeaderInfo* hi, char* dir_path, char* resolved_path) {
	char path[100];
	memset(path, 0, sizeof(path));
	strcat(path, dir_path);
	int len = strlen(path);
	if (path[len-1] != '/') {
		path[len] = '/';
		path[len+1] = '\0';
	}
	strcat(path, hi->host);
	strcat(path, hi->url);
	resolved_path = realpath(path, resolved_path);
}

bool target_file_is_in_domain(HeaderInfo *hi, const char* dir_path, char *path) {
	int len = strlen(dir_path);
	if (memcmp(path,dir_path,len) == 0) {
		char* path2 = path + len;
		if (dir_path[len-1] != '/') path2 += 1;
		if (memcmp(path2,hi->host,strlen(hi->host)) == 0) {
			return true;
		}
		return false;
	}
	return false;
}

bool is_dir(const char* path) {
    struct stat st;
    if (stat(path, &st) < 0) {
		return false;
	}
    return S_ISDIR(st.st_mode);
}

void get_extension(char* path, char* ext) {
	char* res = strrchr(path,'.');
    if (!res) strcat(ext, "application/octet_stream");
    else if (memcmp(res+1,"txt",3) == 0) strcat(ext, "text/plain");
    else if (memcmp(res+1,"css",3) == 0) strcat(ext, "text/css");
    else if (memcmp(res+1,"html",3) == 0) strcat(ext, "text/html");
    else if (memcmp(res+1,"jpg",3) == 0) strcat(ext, "image/jpg");
    else if (memcmp(res+1,"jpeg",3) == 0) strcat(ext, "image/jpeg");
    else if (memcmp(res+1,"png",3) == 0) strcat(ext, "image/png");
    else if (memcmp(res+1,"pdf",3) == 0) strcat(ext, "application/pdf");
    else strcat(ext,res+1);
}

HeaderInfo* extractHeaderInfo(char* buffer)
{
	HeaderInfo *hi = NULL;
	hi = malloc(sizeof(HeaderInfo));
	if (!hi) {
		ERROR("Cannot alloc memory for HeaderInfo struct");
        return NULL;
    }
    memset(hi, 0, sizeof(HeaderInfo));

	if (memcmp(buffer, "GET", strlen("GET")) == 0) {
		buffer += 4;
		size_t url_len = strcspn(buffer, " ");
		memcpy(hi->url, buffer, url_len);
		hi->url[url_len] = '\0';
		buffer += url_len + 1;
		size_t ver_len = strcspn(buffer, "\r\n");
		buffer += ver_len + 2;
		int conn_len = strlen("Connection: ");
		int host_len = strlen("Host: ");
		bool host_found = false;
		bool connection_found = false;
		while (! (buffer[0] == '\r' && buffer[1] == '\n')) {
			if (memcmp(buffer, "Connection: ",conn_len) == 0) {
				connection_found = true;
				if (memcmp(buffer+conn_len, "keep-alive", strlen("keep-alive")) == 0)
					hi->connection = true;
				else if (memcmp(buffer+conn_len, "close", strlen("close")) == 0)
					hi->connection = false;
				else {
					fprintf(stderr, "Bad Connection property format in HTTP Header\n");
					return NULL;
				}
			}
			else if (memcmp(buffer, "Host: ", host_len) == 0) {
				host_found = true;
				size_t name_len = strcspn(buffer+host_len, ":");
				if (name_len > 20) {
					fprintf(stderr, "Bad Host property format in HTTP Header\n");
					return NULL;
				}
				memcpy(hi->host, buffer+host_len, name_len);
				hi->host[name_len] = '\0';
				size_t port_len = strcspn(buffer+host_len+name_len+1, "\r\n");
				if (port_len > 6) {
					fprintf(stderr, "Bad Host property format in HTTP Header\n");
					return NULL;
				}
				char port_buffer[10];
				memcpy(port_buffer, buffer+host_len+name_len+1, port_len);
				port_buffer[port_len] = '\0';
				hi->port = atoi(port_buffer);
			}
			size_t property_len = strcspn(buffer, "\r\n");
			buffer += property_len + 2;
		}
		if (connection_found && host_found) {
			return hi;
		}
		else {
			fprintf(stderr, "Connection or Host property was not found\n");
			return NULL;
		}
	}
	else {
		fprintf(stderr, "Unknown format of HTTP Header\n");
		return NULL;
	}
}