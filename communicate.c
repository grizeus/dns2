#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "communicate.h"

ssize_t send_to(int fd, const char* msg, ssize_t msg_len, struct sockaddr_in* addr) {

    return sendto(fd, msg, msg_len, 0, (const struct sockaddr*)addr, sizeof(*addr));
}

char* receive_from(int fd, struct sockaddr_in* client_addr, ssize_t* recv_len) {

    char buffer[MAX_BUFF_SIZE];

    socklen_t client_addr_len = sizeof(*client_addr);
    *recv_len = recvfrom(fd, buffer, MAX_BUFF_SIZE, 0, (struct sockaddr*)client_addr, &client_addr_len);

    if (*recv_len == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("Receive failed");
        }
        return NULL;
    }

    printf("Received from %s:%d\n", inet_ntoa(client_addr->sin_addr), htons(client_addr->sin_port));

    char* msg = malloc(*recv_len);
    memcpy(msg, buffer, *recv_len);

    return msg;
}