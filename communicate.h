#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include <netinet/in.h>
#include <sys/types.h>

#define MAX_BUFF_SIZE 1024

char *receive_from(int fd, struct sockaddr_in *client_addr, ssize_t *recv_len);
ssize_t send_to(int fd, const char *msg, ssize_t msg_len,
                struct sockaddr_in *addr);

#endif // !COMMUNICATE_H
