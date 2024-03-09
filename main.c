#include <stdint.h>
#include <stdio.h>
#include <netinet/in.h>

#include "str_vector.h"
#include "string.h"
#include "file_parser.h"
#include "dns_parser.h"

typedef struct map map_t;
struct map {

    uint16_t transactionID;
    struct sockaddr_in client;
};

struct client_message {

    string_t message;
    struct sockaddr_in address;
};

int main(int argc, char** argv)
{
    map_t clients;
    char* upstream_addr;
    char** black_list;
    initialize("file.ini", &black_list, &upstream_addr);
    
    while (1)
    {
        if (client_message = receive_from_client())
        {
            dns_name = get_dns(client_message->message);
            if (in_list(dns_name, black_list)
            {
                send_to_client(error, client_message->address);
                continue;
            }
            if (send_to_upstream(upstream_addr, client_message->message))
                // add client id and address to map
                add_data(&clients, client_message->address, get_transaction_id(client_message->message));
            continue;
        }
        else if (message = receive_from_upstream())
        {
            client_addr = find_data(&clients, get_transaction_id(message));
            if (send_to_client(message, clinet_addr))
                remove_data(&clients, get_transaction_id(message));
            continue;
        }
        else
        {
            // if nothing happens programm will eat up 100% of cpu
        }
    }
    return 0;
}
