#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include <netinet/in.h>
#include <sys/types.h>

#include "binary_string.h"

#define MAX_BUFF_SIZE 1024

binary_string_t receive_from(int fd, struct sockaddr_in *client_addr,
                             char *buffer);
ssize_t send_to(int fd, binary_string_t *msg, struct sockaddr_in *addr);

#endif // !COMMUNICATE_H
