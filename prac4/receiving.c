// Filip Marcinek 282905

#include "receiving.h"

int get_data(int fd, char *buffer, int buffer_size)
{
	struct timeval tv; tv.tv_sec = 1; tv.tv_usec = 0;		// pozostały czas
	char *buff_ptr = buffer;
	int total_bytes_read = 0;
	while (total_bytes_read < buffer_size) {
		fprintf(stderr,"DEBUG: current value of tv = %.3f\n", tv.tv_sec + tv.tv_usec * 1.0 / 1000000);

		// Czekamy az bedzie cos w gniezdzie
		fd_set descriptors;
		FD_ZERO (&descriptors);
		FD_SET (fd, &descriptors);
		int ready = select(fd+1, &descriptors, NULL, NULL, &tv);
		if (ready < 0)
			ERROR("select error");
		if (ready == 0) return -2; 		// timeout

		// Odczytujemy dane z gniazda
		int bytes_read = recv(fd, buff_ptr, buffer_size - total_bytes_read, 0);
        return bytes_read;
	}
	return 0;
}