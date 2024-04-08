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
void out_map(list_t* data);
void out_list(record_t* data);
void int_list_out(record_t data);
void record_map_out(list_t* data);
struct sockaddr_in* get_address(list_t* head, binary_string_t* key);
void client_remover(list_t** head, binary_string_t* query);

int test(int pass, const char* msg, const char* file, int line) {

    if (pass) {
        printf("\033[0;32m[%s] PASSED\033[0;37m\n", msg);
    } else {
        printf("\033[0;31m[%s] FAILED in %s line %d\033[0;37m\n", msg, file, line);
    }

    return pass;
}

record_t* create_rcrd(const struct sockaddr_in* address, const uint8_t* query, size_t size) {

    record_t* new_record = (record_t*)malloc(sizeof(record_t));
    if (new_record == NULL) {
        return NULL;
    }
    
    new_record->address = *address;
    new_record->query = binary_string_create(query, size);

    return new_record;
}

list_t* create_rcrd_list(const struct sockaddr_in* address, const uint8_t* query, size_t size) {

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
    
    printf("\033[1;34mlinked list test\033[0;37m\n");
    printf("\033[0;90madd to list\033[0;37m\n");
    {
        list_t* head = NULL;

        struct sockaddr_in client;
        client.sin_family = AF_INET;
        client.sin_port = htons(8001);
        if (inet_pton(AF_INET, "127.0.0.1", &client.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t* str[] = {0x26, 0xa9, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
        size_t size = 9;
        record_t* rec1 = create_rcrd(&client, str, size);

        list_add(&head, rec1);
        list_add(&head, rec1);
        list_add(&head, rec1);

        TEST(head != NULL);
        TEST(head->next != NULL);
        TEST(head->next->next != NULL);
        list_clear(&head);
    }
    printf("\033[0;90mdelete from list\033[0;37m\n");
    {
        list_t* head = NULL;

        struct sockaddr_in client1, client2;
        client1.sin_family = AF_INET;
        client1.sin_port = htons(8001);
        if (inet_pton(AF_INET, "127.0.0.1", &client1.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t query1[] = {0x26, 0xa9, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
        record_t* rec1 = create_rcrd(&client1, query1, sizeof(query1)/sizeof(query1[0]));

        client2.sin_family = AF_INET;
        client2.sin_port = htons(8003);
        if (inet_pton(AF_INET, "127.0.0.1", &client2.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t query2[] = {0x26, 0xa9, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xfe};
        record_t* rec2 = create_rcrd(&client2, query2, sizeof(query2)/sizeof(query2[0]));

        list_add(&head, rec1);
        list_add(&head, rec2);
        list_add(&head, rec1);
        list_add(&head, rec1);
        list_add(&head, rec1);
        list_add(&head, rec1);
        list_iterate(head, out_list);
        TEST(head != NULL);
        TEST(head->next != NULL);
        TEST(head->next->next != NULL);
        
        binary_string_t query_key = binary_string_create(query2, sizeof(query2)/sizeof(query2[0]));
        list_delete(&head, &query_key, compare_record);
        // TEST(head->next->next == NULL);
        list_iterate(head, out_list);
        list_t* res = list_find(&head, &query_key, compare_record);
        TEST(res == NULL);
        list_clear(&head);
    }
    printf("\033[0;90mclear list\033[0;37m\n");
    {
        list_t* head = NULL;

        for(size_t i = 0; i < 3; ++i) {
            list_add(&head, i);
        }
        
        TEST(head != NULL);
        TEST(head->next != NULL);
        TEST(head->next->next != NULL);

        list_clear(&head);
        TEST(head == NULL);
    }
    printf("\033[1;34mmap test\033[0;37m\n");
    printf("\033[0;90madd to map\033[0;37m\n");
    {
        map_t* map = map_create();
        struct sockaddr_in client;
        client.sin_family = AF_INET;
        client.sin_port = htons(8001);
        if (inet_pton(AF_INET, "127.0.0.1", &client.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t* str[] = {0x26, 0xa9, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
        size_t size = 9;
        list_t* rec1 = create_rcrd_list(&client, str, size);
        list_t* rec2 = create_rcrd_list(&client, str, size);
        list_t* rec3 = create_rcrd_list(&client, str, size);
        map_add(map, 300, rec1, list_add);
        map_add(map, 20, rec2, list_add);
        map_add(map, 69, rec3, list_add);

        TEST(map->root != map->sentinel);
        puts("Balance test");
        TEST(map->root->key == 69);
        map_clear(map, list_clear);
    }
    printf("\033[0;90madd to map primitive\033[0;37m\n");
    {
        map_t* map = map_create();
        struct sockaddr_in client;
        client.sin_family = AF_INET;
        client.sin_port = htons(8001);
        if (inet_pton(AF_INET, "127.0.0.1", &client.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t* str[] = {0x26, 0xa9, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
        size_t size = 9;
        // list_t* rec1 = create_rcrd_list(&client, str, size);
        record_t* rec1 = create_rcrd(&client, str, size);
        map_add(map, 300, rec1, NULL);
        map_add(map, 20, rec1, NULL);
        map_add(map, 69, rec1, NULL);

        TEST(map != NULL);
        map_clear(map, NULL);

        map_add(map, 1, 10, NULL);
        int result = map_find(map, 1);
        TEST(result == 10);
    }
    printf("\033[0;90msearch in map\033[0;37m\n");
    {
        map_t* map = map_create();
        struct sockaddr_in client_1, client_2;
        client_1.sin_family = AF_INET;
        client_1.sin_port = htons(8001);
        if (inet_pton(AF_INET, "127.0.0.1", &client_1.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t query1[] = {0x26, 0xa9, 0x01, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0xff};
        size_t size = 10;
        list_t* rec1 = create_rcrd_list(&client_1, &query1, size);
        list_t* rec2 = create_rcrd_list(&client_1, &query1, size);
        list_t* rec3 = create_rcrd_list(&client_1, &query1, size);
        map_add(map, 1, rec1, list_add_node);
        map_add(map, 2, rec2, list_add_node);
        map_add(map, 3, rec3, list_add_node);

        int key = 2;
        list_t* res_list = map_find(map, key);
        key = 5;
        list_t* not_in_map = map_find(map, key);
        TEST(res_list != NULL);
        TEST(not_in_map == NULL);

        binary_string_t query_key1 = binary_string_create(query1, size);

        list_t* res_node1 = list_find(res_list, &query_key1, compare_record);
        if (res_node1 != NULL) {
            struct sockaddr_in res_addr1 = ((record_t*)res_node1->data)->address;
            binary_string_t res_query = ((record_t*)res_node1->data)->query;
            TEST(memcmp(res_query.data, query_key1.data, size) == 0);
            TEST(ntohs(res_addr1.sin_port) == ntohs(client_1.sin_port));
        }

        map_clear(map, list_clear);
        binary_string_destroy(&query_key1);
    }
    printf("\033[0;90msearch in map with separate chaining\033[0;37m\n");
    {
        map_t* map = map_create();
        struct sockaddr_in client_1, client_2;
        client_1.sin_family = AF_INET;
        client_1.sin_port = htons(8001);
        if (inet_pton(AF_INET, "127.0.0.1", &client_1.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t query1[] = {0x26, 0xa9, 0x01, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0xff};
        size_t size = 10;
        list_t* rec1 = create_rcrd_list(&client_1, &query1, size);
        list_t* rec2 = create_rcrd_list(&client_1, &query1, size);
        list_t* rec3 = create_rcrd_list(&client_1, &query1, size);
        map_add(map, 1, rec1, list_add_node);
        map_add(map, 2, rec2, list_add_node);
        map_add(map, 3, rec3, list_add_node);
        
        client_2.sin_family = AF_INET;
        client_2.sin_port = htons(8003);
        if (inet_pton(AF_INET, "127.0.0.1", &client_2.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t query2[] = {0x26, 0xa9, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xfe};
        list_t* rec2_2 = create_rcrd_list(&client_2, &query2, size);
        map_add(map, 2, rec2_2, list_add_node);

        int key = 2;
        list_t* res_list = map_find(map, key);
        key = 5;
        list_t* not_in_map = map_find(map, key);
        TEST(res_list != NULL);
        TEST(not_in_map == NULL);

        binary_string_t query_key1 = binary_string_create(query1, size);
        binary_string_t query_key2 = binary_string_create(query2, size);

        list_t* res_node1 = list_find(res_list, &query_key1, compare_record);
        if (res_node1 != NULL) {
            struct sockaddr_in res_addr1 = ((record_t*)res_node1->data)->address;
            binary_string_t res_query = ((record_t*)res_node1->data)->query;
            TEST(memcmp(res_query.data, query_key1.data, size) == 0);
            TEST(ntohs(res_addr1.sin_port) == ntohs(client_1.sin_port));
        }

        list_t* res_node2 = list_find(res_list, &query_key2, compare_record);
        if (res_node2 != NULL) {
            struct sockaddr_in res_addr2 = ((record_t*)res_node2->data)->address;
            TEST(ntohs(res_addr2.sin_port) == ntohs(client_2.sin_port));
        }

        map_clear(map, list_clear);
        binary_string_destroy(&query_key1);
        binary_string_destroy(&query_key2);
    }
    printf("\033[0;90mtest get_address func\033[0;37m\n");
    {
        map_t* map = map_create();
        struct sockaddr_in client_1, client_2;
        client_1.sin_family = AF_INET;
        client_1.sin_port = htons(8001);
        if (inet_pton(AF_INET, "127.0.0.1", &client_1.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t query1[] = {0x26, 0xa9, 0x01, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0xff};
        size_t size = 10;
        list_t* rec1 = create_rcrd_list(&client_1, &query1, size);
        list_t* rec2 = create_rcrd_list(&client_1, &query1, size);
        list_t* rec3 = create_rcrd_list(&client_1, &query1, size);
        map_add(map, 1, rec1, list_add_node);
        map_add(map, 2, rec2, list_add_node);
        map_add(map, 3, rec3, list_add_node);
        
        client_2.sin_family = AF_INET;
        client_2.sin_port = htons(8003);
        if (inet_pton(AF_INET, "127.0.0.1", &client_2.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t query2[] = {0x26, 0xa9, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xfe};
        list_t* rec2_2 = create_rcrd_list(&client_2, &query2, size);
        map_add(map, 2, rec2_2, list_add_node);

        int key = 2;
        list_t* res_list = map_find(map, key);
        
        binary_string_t query_key1 = binary_string_create(query1, size);
        binary_string_t query_key2 = binary_string_create(query2, size);

        struct sockaddr_in* result_addr = get_address(res_list, &query_key1);
        TEST(result_addr != NULL);
        TEST(ntohs(result_addr->sin_port) == ntohs(client_1.sin_port));
        result_addr = get_address(res_list, &query_key2);
        TEST(ntohs(result_addr->sin_port) == ntohs(client_2.sin_port));

        map_clear(map, list_clear);
        binary_string_destroy(&query_key1);
        binary_string_destroy(&query_key2);
    }
    printf("\033[0;90mremove from map\033[0;37m\n");
    {
        map_t* map = map_create();
        struct sockaddr_in client_1, client_2;
        client_1.sin_family = AF_INET;
        client_1.sin_port = htons(8001);
        if (inet_pton(AF_INET, "127.0.0.1", &client_1.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t query1[] = {0x26, 0xa9, 0x01, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0xff};
        list_t* rec1 = create_rcrd_list(&client_1, &query1, sizeof(query1) / sizeof(query1[0]));
        list_t* rec2 = create_rcrd_list(&client_1, &query1, sizeof(query1) / sizeof(query1[0]));
        list_t* rec3 = create_rcrd_list(&client_1, &query1, sizeof(query1) / sizeof(query1[0]));
        map_add(map, 1, rec1, list_add_node);
        map_add(map, 2, rec2, list_add_node);
        map_add(map, 3, rec3, list_add_node);
        
        client_2.sin_family = AF_INET;
        client_2.sin_port = htons(8003);
        if (inet_pton(AF_INET, "127.0.0.1", &client_2.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        uint8_t query2[] = {0x26, 0xa9, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xfe};
        list_t* rec2_2 = create_rcrd_list(&client_2, &query2, sizeof(query2) / sizeof(query2[0]));
        map_add(map, 2, rec2_2, list_add_node);

        int key = 2;
        list_t* res_list = map_find(map, key);
        map_iterate(map, out_map);
        TEST(res_list != NULL);
        
        binary_string_t query_key1 = binary_string_create(query1, sizeof(query1) / sizeof(query1[0]));
        binary_string_t query_key2 = binary_string_create(query2, sizeof(query2) / sizeof(query2[0]));

        struct sockaddr_in* result_addr = get_address(res_list, &query_key1);
        TEST(result_addr != NULL);
        TEST(ntohs(result_addr->sin_port) == ntohs(client_1.sin_port));
        result_addr = get_address(res_list, &query_key2);
        TEST(result_addr != NULL);
        TEST(ntohs(result_addr->sin_port) == ntohs(client_2.sin_port));

        map_delete(map, key, client_remover, &query_key1, NULL);
        res_list = map_find(map, key);
        TEST(res_list != NULL);
        result_addr = get_address(res_list, &query_key1);
        TEST(result_addr == NULL);
        result_addr = get_address(res_list, &query_key2);
        TEST(result_addr != NULL);
        TEST(ntohs(result_addr->sin_port) == ntohs(client_2.sin_port));


        key = 1;
        map_delete(map, key, client_remover, &query_key1, NULL);
        map_iterate(map, out_map); // segfault?
        res_list = map_find(map, key);
        TEST(res_list == NULL);

        map_clear(map, list_clear);
        binary_string_destroy(&query_key1);
        binary_string_destroy(&query_key2);
    }
    if (!failed) {
        return 0;
    }

    abort();
}

void int_list_out(record_t data) {
    printf("%d %s \n", ntohs(data.address.sin_port), data.query);
}

void out_list(record_t* data) {
    printf("%d %s \n", ntohs(data->address.sin_port), data->query);
}

void out_map(list_t* data) {

    list_iterate(data, out_list);
}

void record_map_out(list_t* data) {

    list_iterate(data, int_list_out);
}

struct sockaddr_in* get_address(list_t* head, binary_string_t* key) {

    // if list have only one node
    if (head->next == NULL) {
        record_t* rec_ptr = head->data;
        if((key->size == rec_ptr->query.size) &&
                memcmp(key->data, rec_ptr->query.data, key->size) == 0) {
            return &rec_ptr->address;
        }
        return NULL;
    }

    list_t* searching_node = list_find(head, key, compare_record);
    return &((record_t*)searching_node->data)->address;
}

void client_remover(list_t** head, binary_string_t* query) {

    list_delete(head, query, compare_record);
}