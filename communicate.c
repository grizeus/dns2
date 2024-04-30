#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "communicate.h"

ssize_t send_to(int fd, const char *msg, ssize_t msg_len,
                struct sockaddr_in *addr) {

  return sendto(fd, msg, msg_len, 0, (const struct sockaddr *)addr,
                sizeof(*addr));
}

char *receive_from(int fd, struct sockaddr_in *client_addr, ssize_t *recv_len) {

  static char buffer[MAX_BUFF_SIZE];
  memset(&buffer, 0, MAX_BUFF_SIZE);
  socklen_t client_addr_len = sizeof(*client_addr);
  *recv_len = recvfrom(fd, buffer, MAX_BUFF_SIZE, 0,
                       (struct sockaddr *)client_addr, &client_addr_len);

  if (*recv_len == -1) {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("Receive failed");
    }
    return NULL;
  }
  FILE *log = fopen("log.txt", "a");
  fprintf(log, "Received from %s:%d with len %zd\n",
          inet_ntoa(client_addr->sin_addr), htons(client_addr->sin_port),
          *recv_len);
  printf("Received from %s:%d with len %zd\n", inet_ntoa(client_addr->sin_addr),
         htons(client_addr->sin_port), *recv_len);
  printf("%s\n", buffer);
  fclose(log);

  return buffer;
}
