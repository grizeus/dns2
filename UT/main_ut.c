#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../binary_string.h"
#include "../query_record.h"
#include "../map.h"
#include "../linked_list.h"

static int failed = 0;

int test(int pass, const char* msg, const char* file, int line) {

    if (pass) {
        printf("\033[0;32m[%s] PASSED\033[0;37m\n", msg);
    } else {
        printf("\033[0;31m[%s] FAILED in %s line %d\033[0;37m\n", msg, file, line);
    }

    return pass;
}

list_t* create_record_ut(const struct sockaddr_in* address, const uint8_t* query, size_t size) {

    record_t* new_record = (record_t*)malloc(sizeof(record_t));
    if (new_record == NULL) {
        return NULL;
    }
    
    new_record->address = *address;
    new_record->query = binary_string_create(query, size);
    list_t* new_list = list_new(new_record);

    return new_list;
}

#define TEST(EX) (void)(test (EX, #EX, __FILE__, __LINE__))

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
        map_t* map = NULL;
        struct sockaddr_in client;
        client.sin_family = AF_INET;
        client.sin_port = htons(8001);
        if (inet_pton(AF_INET, "127.0.0.1", &client.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t* str[] = {0x26, 0xa9, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
        size_t size = 9;
        list_t* rec1 = create_record_ut(&client, str, size);
        map_add(&map, 300, rec1, list_add);
        map_add(&map, 20, rec1, list_add);
        map_add(&map, 69, rec1, list_add);

        TEST(map != NULL);
        puts("Balance test");
        TEST(map->key == 69);
        map_clear(&map, NULL);
        printf("\n");
    }
    printf("\033[0;90msearch in map\033[0;37m\n");
    {
        map_t* map = NULL;
        struct sockaddr_in client_1, client_2;
        client_1.sin_family = AF_INET;
        client_1.sin_port = htons(8001);
        if (inet_pton(AF_INET, "127.0.0.1", &client_1.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t query1[] = {0x26, 0xa9, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xff};
        size_t size = 10;
        list_t* rec1 = create_record_ut(&client_1, &query1, size);
        map_add(&map, 1, rec1, list_add);
        map_add(&map, 2, rec1, list_add);
        map_add(&map, 3, rec1, list_add);

        client_2.sin_family = AF_INET;
        client_2.sin_port = htons(8003);
        if (inet_pton(AF_INET, "127.0.0.1", &client_2.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t query2[] = {0x26, 0xa9, 0x02, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0xff};
        list_t* rec2 = create_record_ut(&client_2, &query2, size);
        map_add(&map, 2, rec2, list_add);

        int key = 2;
        list_t* res_list = map_find(map, key);
        key = 5;
        list_t* not_in_map = map_find(map, key);
        TEST(res_list != NULL);
        TEST(not_in_map == NULL);

        binary_string_t query_key1 = binary_string_create(query1, size);
        for (int i = 0; i < query_key1.size; ++i) {
            printf("%02x ", query_key1.data[i] & 0xFF);
        }
        putchar('\n');
        binary_string_t query_key2 = binary_string_create(query2, size);
        for (int i = 0; i < query_key2.size; ++i) {
            printf("%02x ", query_key2.data[i] & 0xFF);
        }
        putchar('\n');

        list_t* res_node1 = list_find(res_list, &query_key1, compare_record);
        list_t* res_node2 = list_find(res_list, &query_key2, compare_record);
        if (res_node2 != NULL) {
            struct sockaddr_in res_addr2 = ((record_t*)res_node2->data)->address;
            TEST(ntohs(res_addr2.sin_port) == ntohs(client_2.sin_port));
            printf("%d %d\n", ntohs(res_addr2.sin_port), ntohs(client_2.sin_port));
        }
        struct sockaddr_in res_addr1 = ((record_t*)res_node1->data)->address;

        TEST(ntohs(res_addr1.sin_port) == ntohs(client_1.sin_port));
        printf("%d %d\n", ntohs(res_addr1.sin_port), ntohs(client_1.sin_port));
        map_clear(&map, NULL);
        binary_string_destroy(&query_key1);
        binary_string_destroy(&query_key2);
        printf("\n");
    }
    if (!failed) {
        return 0;
    }

    abort();
}
