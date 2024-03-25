#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../string.h"
#include "../str_vector.h"
#include "../map.h"

static int failed = 0;

int test(int pass, const char* msg, const char* file, int line) {

    if (pass) {
        printf("\033[0;32m[%s] PASSED\033[0;37m\n", msg);
    } else {
        printf("\033[0;31m[%s] FAILED in %s line %d\033[0;37m\n", msg, file, line);
    }

    return pass;
}

#define TEST(EX) (void)(test (EX, #EX, __FILE__, __LINE__))

int main() {
    printf("\033[1;34mstring_t tests\033[0;37m\n");
    printf("\033[0;90mcreate string\033[0;37m\n");
    {
        char* c_str = "test";
        string_t str = create_string(c_str);
        TEST(str.data != NULL);
        TEST(str.size == strlen(c_str));
        TEST(strcmp(str.data, c_str) == 0);
        destroy_string(&str);
    }
    printf("\033[0;90mfree string\033[0;37m\n");
    {
        char* c_str = "test";
        string_t str = create_string(c_str);
        destroy_string(&str);
        TEST(str.data == NULL);
        TEST(str.size == 0);
    }
    
    printf("\033[1;34mstr_vector tests\033[0;37m\n");
    printf("\033[0;90mcreate empty vector\033[0;37m\n");
    {
        vector_t vec = create_vector();
        TEST(vec.data != NULL && vec.size ==  0 && vec.capacity == INIT_CAPACITY);
        destroy_vector(&vec);
    }
    printf("\033[0;90mpushback string_t\033[0;37m\n");
    {
        vector_t vec = create_vector();
        char c_str1[] = "test1";
        char c_str2[] = "test2";
        string_t str1 = create_string(c_str1);
        string_t str2 = create_string(c_str2);
        push_back(&vec, &str1);
        push_back(&vec, &str2);
        TEST(vec.size == 2);
        TEST(strcmp(vec.data[0].data, str1.data) == 0);
        TEST(strcmp(vec.data[1].data, str2.data) == 0);

        destroy_string(&str1);
        destroy_string(&str2);
        destroy_vector(&vec);
    }
    printf("\033[0;90mdestroy empty vector\033[0;37m\n");
    {
        vector_t vec = create_vector();
        string_t str = { .data = "test", .size =  4 };
        push_back(&vec, &str);
        destroy_vector(&vec);
        TEST(vec.data == NULL && vec.size ==  0 && vec.capacity ==  0);
    }

    printf("\033[0;90mpop_back string\033[0;37m\n");
    {
        vector_t vec = create_vector();
        char* c_str1 = "test1";
        char* c_str2 = "test2";
        string_t str1 = create_string(c_str1);
        string_t str2 = create_string(c_str2);
        push_back(&vec, &str1);
        push_back(&vec, &str2);
        TEST(vec.size == 2);
        TEST(strcmp(vec.data[0].data, str1.data) == 0);
        TEST(strcmp(vec.data[1].data, str2.data) == 0);
        pop_back(&vec);
        TEST(vec.size == 1);
        TEST(strcmp(vec.data[0].data, str1.data) == 0);

        destroy_string(&str1);
        destroy_string(&str2);
        destroy_vector(&vec);

    }
    printf("\033[1;34mmap test\033[0;37m\n");
    printf("\033[0;90minorder traversal\033[0;37m\n");
    {
        map_t map;
        transaction_record_t pair1 = { .transactionID = 1,
                               .client.sin_family = AF_INET,
                               .client.sin_port = htons(8081)};
        if (inet_pton(AF_INET, "127.0.0.1", &pair1.client.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        transaction_record_t pair2= { .transactionID = 2,
                               .client.sin_family = AF_INET,
                               .client.sin_port = htons(8082)};
        if (inet_pton(AF_INET, "127.0.0.1", &pair2.client.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        transaction_record_t pair3 = { .transactionID = 3,
                               .client.sin_family = AF_INET,
                               .client.sin_port = htons(8079)};
        if (inet_pton(AF_INET, "127.0.0.1", &pair3.client.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        insert_into_map(&map, pair1);
        insert_into_map(&map, pair2);
        insert_into_map(&map, pair3);

        printf("Inorder Traversal (Key-Value Pairs):\n");
        map_traversal(map.root);
        destroy_map(&map);
        printf("\n");
    }
    printf("\033[0;90msearch in map\033[0;37m\n");
    {
        map_t map;
        transaction_record_t pair1 = { .transactionID = 1,
                               .client.sin_family = AF_INET,
                               .client.sin_port = htons(8081)};
        if (inet_pton(AF_INET, "127.0.0.1", &pair1.client.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        transaction_record_t pair2= { .transactionID = 2,
                               .client.sin_family = AF_INET,
                               .client.sin_port = htons(8082)};
        if (inet_pton(AF_INET, "127.0.0.1", &pair2.client.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        transaction_record_t pair3 = { .transactionID = 3,
                               .client.sin_family = AF_INET,
                               .client.sin_port = htons(8079)};
        if (inet_pton(AF_INET, "127.0.0.1", &pair3.client.sin_addr) != 1) {
            perror("inet_pton");
            return 1;
        }
        insert_into_map(&map, pair1);
        insert_into_map(&map, pair2);
        insert_into_map(&map, pair3);
        map_node_t* required = search(&map, 1);
        map_node_t* not_in_map= search(&map, 5);
        TEST(ntohs(required->data.client.sin_port) == ntohs(pair1.client.sin_port));
        TEST(not_in_map == NULL);
        destroy_map(&map);
    }
    if (!failed) {
        return 0;
    }

    abort();
}
