#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_parser.h"

#define BUFFER_SIZE 256

/**
 * Parses a line of text based on a given key and returns a vector of strings.
 * Each string represents a value associated with the key separated by commas.
 *
 * @param key The key to look for in the line.
 * @param line string (an array of chars) of text to parse.
 * @return Array of char* pointers containing the parsed values.
 */
static char** get_list(const char* key, char* line);
/**
 * Parses a line of text based on a given key and returns a string.
 *
 * @param key The key to look for in the line.
 * @param line string (an array of chars) of text to parse.
 * @return A string containing the parsed value.
 */
static char* get_string(const char* key, char* line);

static char* trim_whitespaces(char* str) {

    if (str == NULL || *str == '\0') {
        return str;
    }

    char* start = str;
    char* end = str + strlen(str) - 1;

    while (isspace(*start)) {
        start++;
    }

    while (end > start && isspace(*end)) {
        end--;
    }

    *(end + 1) = '\0';

    return start;
}

static void count_size(const char* key, const char* line, int* count) {

    *count = 0;
    char* buf = strdup(line);
    char* token = strtok(buf, "=");
    if (token == NULL || strcmp(token, key) != 0) {
        fprintf(stderr, "Key is not found\n");
        return;
    }

    while ((token = strtok(NULL, ",")) != NULL) {
        (*count)++;
    }
}

static char** get_list(const char* key, char* line) {

    int count;
    count_size(key, line, &count);
    char** values = malloc((count + 1) * sizeof(char*));
    if (values == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    // reset count to parse
    count = 0;

    char* current_token = strtok(line, "=");

    // Check if the first token matches the expected key
    if (current_token == NULL || strcmp(current_token, key) != 0) {
        // If the key does not match, free the memory and return an empty vector
        fprintf(stderr, "Key \"%s\" is not found\n", key);
        return NULL;
    }


    // Continue tokenizing the remaining part of the line using comma as a delimiter
    while ((current_token = strtok(NULL, ",")) != NULL) {
        count++;
        values[count-1] = trim_whitespaces(current_token);
    }

    values[count] = NULL;

    return values;

}

static char* get_string(const char* key, char* line) {

    char* current_token = strtok(line, "=");

    // Check if the first token matches the expected key
    if (current_token == NULL || strcmp(current_token, key) != 0) {
        // If the key does not match, free the memory and return an empty vector
        fprintf(stderr, "Key \"%s\" is not found\n", key);
        return NULL;
    }

    current_token = strtok(NULL, "_");
    char* trimmed_str = trim_whitespaces(current_token);

    // Allocate memory for final_str (including space for null terminator)
    char* final_str = (char*)malloc(strlen(trimmed_str) + 1);

    // Check if memory allocation is successful
    if (final_str == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Copy the trimmed string to final_str
    strcpy(final_str, trimmed_str);

    return final_str;
}

void initialize(const char* filename, char** black_list, server_config_t* config) {

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "File %s could not open\n", filename);
        return;
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), file)) {
        if (black_list == NULL) {
            black_list = get_list("Domains", buffer);
        } else if (config->upstream_name == NULL) {
            config->upstream_name = get_string("Upstream", buffer);
        } else if (config->local_address == NULL) {
            config->upstream_name = get_string("Local_addr", buffer);
        } else if (config->port == 0) {
            config->port = atoi(get_string("Port", buffer));
        } else if (config->dns_port == 0) {
            config->port = atoi(get_string("DNS_port", buffer));
        }


        if (black_list != NULL &&
            config->upstream_name != NULL && 
            config->local_address != NULL && 
            config->port != 0 &&
            config->dns_port != 0) {

            break;
        }
    }

    fclose(file);
}

int in_list(const char* target, char** list) {

    while (*list != NULL) {
        if (strcmp(target, *list) == 0) {
            return 1;
        }
    }

    return 0;
}