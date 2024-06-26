#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "../binary_string.h"
#include "../dns_parser.h"
#include "../file_parser.h"
#include "../map.h"
#include "../utility.h"

static int failed = 0;

int test(int pass, const char *msg, const char *file, int line) {

  if (pass) {
    printf("\033[0;32m[%s] PASSED\033[0;37m\n", msg);
  } else {
    printf("\033[0;31m[%s] FAILED in %s line %d\033[0;37m\n", msg, file, line);
  }

  return pass;
}

void create_ini(const char *filename) {

  FILE *new_file;
  char *data[] = {"Domains = www.vk.com, www.x.com\nUpstream = 1.1.1.1\n"};

  new_file = fopen(filename, "w");
  if (new_file == NULL) {
    fprintf(stderr, "Error opening file.\n");
    exit(1);
  }
  size_t lines = sizeof(data) / sizeof(data[0]);
  for (size_t i = 0; i < lines; ++i) {
    if (fputs(data[i], new_file) == EOF) {
      fprintf(stderr, "Error writing to file.\n");
      fclose(new_file);
      exit(1);
    }
  }

  fclose(new_file);
}

#define TEST(EX) (void)(test(EX, #EX, __FILE__, __LINE__))

int main() {
  printf("\033[1;34mstring_t tests\033[0;37m\n");
  printf("\033[0;90mcreate string\033[0;37m\n");
  {
    uint8_t str[] = {0x26, 0xa9, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    size_t size = 9;
    binary_string_t b_str = binary_string_create(str, size);
    TEST(b_str.data != NULL);
    TEST(b_str.size == (sizeof(str) / sizeof(str[0])));
    TEST(memcmp(b_str.data, str, size) == 0);
    binary_string_destroy(&b_str);
  }
  printf("\033[0;90mdeleteproof substrate string\033[0;37m\n");
  {
    uint8_t str[] = {0x26, 0xa9, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    size_t size = 9;
    uint8_t *str_p = (uint8_t *)malloc(size * sizeof(uint8_t));
    memcpy(str_p, str, size);
    binary_string_t b_str = binary_string_create(str_p, size);
    free(str_p);
    str_p = NULL;
    TEST(b_str.data != NULL);
    TEST(b_str.size == 9);
    TEST(memcmp(b_str.data, str, size) == 0);
    binary_string_destroy(&b_str);
  }
  printf("\033[0;90mfree string\033[0;37m\n");
  {
    uint8_t str[] = {0x26, 0xa9, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    size_t size = 9;
    binary_string_t b_str = binary_string_create(str, size);
    binary_string_destroy(&b_str);
    TEST(b_str.data == NULL);
    TEST(b_str.size == 0);
  }
  printf("\033[0;90mcompare string\033[0;37m\n");
  {
    uint8_t str1[] = {0x26, 0xa9, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    uint8_t str2[] = {0x26, 0xaf, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    size_t size = 9;
    binary_string_t b_str1 = binary_string_create(str1, size);
    binary_string_t b_str2 = binary_string_create(str2, size);
    TEST(memcmp(b_str1.data, b_str2.data, size) != 0);
    binary_string_destroy(&b_str1);
    binary_string_destroy(&b_str2);
  }
  printf("\033[1;34mmap test\033[0;37m\n");
  printf("\033[0;90madd to map\033[0;37m\n");
  {
    map_t *map = map_create();
    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_port = htons(8001);
    if (inet_pton(AF_INET, "127.0.0.1", &client.sin_addr) != 1) {
      perror("inet_pton");
      return 1;
    }
    map_add(map, 300, &client, NULL);
    map_add(map, 20, &client, NULL);
    map_add(map, 69, &client, NULL);

    TEST(map->root != map->sentinel);
    puts("Balance test");
    TEST(map->root->key == 69);
    map_clear(map, NULL);
  }
  printf("\033[0;90madd to map primitive\033[0;37m\n");
  {
    map_t *map = map_create();
    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_port = htons(8001);
    if (inet_pton(AF_INET, "127.0.0.1", &client.sin_addr) != 1) {
      perror("inet_pton");
      return 1;
    }
    map_add(map, 300, &client, NULL);
    map_add(map, 20, &client, NULL);
    map_add(map, 69, &client, NULL);

    TEST(map != NULL);
    map_clear(map, NULL);
    int int_val = 10;
    map_add(map, 1, &int_val, NULL);
    int *result = map_find(map, 1);
    TEST(*result == 10);
    result = map_find(map, 2);
    TEST(result == NULL);
  }
  printf("\033[0;90msearch in map\033[0;37m\n");
  {
    map_t *map = map_create();
    struct sockaddr_in client_1;
    client_1.sin_family = AF_INET;
    client_1.sin_port = htons(8001);
    if (inet_pton(AF_INET, "127.0.0.1", &client_1.sin_addr) != 1) {
      perror("inet_pton");
      return 1;
    }
    map_add(map, 1, &client_1, NULL);
    map_add(map, 2, &client_1, NULL);
    map_add(map, 3, &client_1, NULL);

    int key = 2;
    struct sockaddr_in *res = map_find(map, key);
    key = 5;
    struct sockaddr_in *not_in_map = map_find(map, key);
    TEST(res != NULL);
    TEST(not_in_map == NULL);

    map_clear(map, NULL);
  }
  printf("\033[0;90mremove from map\033[0;37m\n");
  {
    map_t *map = map_create();
    struct sockaddr_in client_1;
    client_1.sin_family = AF_INET;
    client_1.sin_port = htons(8001);
    if (inet_pton(AF_INET, "127.0.0.1", &client_1.sin_addr) != 1) {
      perror("inet_pton");
      return 1;
    }
    map_add(map, 1, &client_1, NULL);
    map_add(map, 2, &client_1, NULL);
    map_add(map, 3, &client_1, NULL);

    int key = 2;
    struct sockaddr_in *res = map_find(map, key);
    TEST(res != NULL);
    TEST(ntohs(res->sin_port) == ntohs(client_1.sin_port));

    map_delete(map, key, NULL, NULL, NULL);
    res = map_find(map, key);
    TEST(res == NULL);

    map_clear(map, NULL);
  }
  printf("\033[0;90mparse dns function\033[0;37m\n");
  {
    uint8_t query_ar[] = {0x52, 0xc3, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x03, 0x77, 0x77, 0x77,
                          0x06, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x03,
                          0x63, 0x6f, 0x6d, 0x00, 0x00, 0x1c, 0x00, 0x01};
    binary_string_t query = binary_string_create(query_ar, sizeof(query_ar));
    query_data_t query_data = parse_query(&query);
    uint8_t response_ar[] = {
        0x52, 0xc3, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x77, 0x77, 0x77, 0x06, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x03,
        0x63, 0x6f, 0x6d, 0x00, 0x00, 0x1c, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x1c,
        0x00, 0x01, 0x00, 0x00, 0x01, 0x0f, 0x00, 0x10, 0x2a, 0x00, 0x14, 0x50,
        0x40, 0x1b, 0x08, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04};

    binary_string_t response =
        binary_string_create(response_ar, sizeof(response_ar));
    response_data_t response_data = parse_response(&response);
    TEST(query_data.question_hash == response_data.question_hash);
    TEST(query_data.client_hash == response_data.client_hash);
    TEST(response_data.answer->size == (response.size - query.size));

    binary_string_t new_response =
        build_response(&query, response_data.answer);
    TEST(memcmp(new_response.data, response.data, new_response.size) == 0);
    binary_string_destroy(&new_response);
  }
  printf("\033[0;90mbuild dns response\033[0;37m\n");
  {
    map_t *map = map_create();
    uint8_t response_ar[] = {
        0x52, 0xc3, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x77, 0x77, 0x77, 0x06, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x03,
        0x63, 0x6f, 0x6d, 0x00, 0x00, 0x1c, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x1c,
        0x00, 0x01, 0x00, 0x00, 0x01, 0x0f, 0x00, 0x10, 0x2a, 0x00, 0x14, 0x50,
        0x40, 0x1b, 0x08, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04};

    binary_string_t response =
        binary_string_create(response_ar, sizeof(response_ar));
    response_data_t response_data = parse_response(&response);
    map_add(map, response_data.question_hash, response_data.answer, NULL);

    uint8_t query_ar[] = {0x52, 0xc3, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x03, 0x77, 0x77, 0x77,
                          0x06, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x03,
                          0x63, 0x6f, 0x6d, 0x00, 0x00, 0x1c, 0x00, 0x01};
    binary_string_t query = binary_string_create(query_ar, sizeof(query_ar));
    query_data_t query_data = parse_query(&query);
    binary_string_t *answer_from = map_find(map, query_data.question_hash);
    binary_string_t new_response =
        build_response(&query, answer_from);
    TEST(memcmp(new_response.data, response.data, new_response.size) == 0);
    binary_string_destroy(&query);
    binary_string_destroy(&new_response);
    free(query_data.dns_name);
  }
  printf("\033[0;90mparse file function\033[0;37m\n");
  {
    const char *filename = "config.ini";
    create_ini(filename);

    init_data_t data = initialize(filename);
    TEST(in_list("www.vk.com", data.black_list) == 1);
    TEST(in_list("www.x.com", data.black_list) == 1);
    TEST(in_list("raga.com", data.black_list) == 0);
    for (size_t i = 0; data.black_list[i] != NULL; ++i) {
      free(data.black_list[i]);
    }
    free(data.black_list);
    free(data.upstream);
  }
  if (!failed) {
    return 0;
  }

  abort();
}
