#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

#include "binary_string.h"
#include "communicate.h"

void send_to(int fd, binary_string_t *msg, struct sockaddr_in *addr) {

  fd_set write_fds;
  FD_ZERO(&write_fds);
  FD_SET(fd, &write_fds);
  size_t bytes_remaining = msg->size;
  size_t total_bytes_sent = 0;

  while (bytes_remaining > 0) {
    fd_set cur_write_fds = write_fds;
    int result = select(fd + 1, NULL, &cur_write_fds, NULL, NULL);
    if (result == -1) {
      perror("Select");
      break;
    } else if (result > 0) {
      if (FD_ISSET(fd, &cur_write_fds)) {

        ssize_t bytes_sent =
            sendto(fd, msg->data + total_bytes_sent, bytes_remaining, 0,
                   (const struct sockaddr *)addr, sizeof(*addr));
        if (bytes_sent == -1) {
          if (errno == EAGAIN && errno == EWOULDBLOCK) {
            continue;
          } else {
            perror("sendto");
            break;
          }
        }
        total_bytes_sent += bytes_sent;
        bytes_remaining -= bytes_sent;
      }
    }
  }

  // return sendto(fd, msg->data, msg->size, 0, (const struct sockaddr *)addr,
  //               sizeof(*addr));
}

binary_string_t receive_from(int fd, struct sockaddr_in *client_addr,
                             char *buffer) {

  binary_string_t msg = {NULL, 0};
  memset(buffer, 0, MAX_BUFF_SIZE);
  socklen_t client_addr_len = sizeof(*client_addr);
  ssize_t recv_len = recvfrom(fd, buffer, MAX_BUFF_SIZE, 0,
                              (struct sockaddr *)client_addr, &client_addr_len);

  if (recv_len == -1) {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("Receive failed");
    }
    return msg;
  }

  FILE *log = fopen("log.txt", "a");
  fprintf(log, "Received from %s:%d with len %zd\n",
          inet_ntoa(client_addr->sin_addr), htons(client_addr->sin_port),
          recv_len);
  printf("Received from %s:%d with len %zd\n", inet_ntoa(client_addr->sin_addr),
         htons(client_addr->sin_port), recv_len);
  printf("%s\n", buffer);
  fclose(log);

  msg = binary_string_create((uint8_t *)buffer, recv_len);

  return msg;
}
