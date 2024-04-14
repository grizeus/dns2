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

static char* trim_whitespaces(char* str) {

    if (str == NULL || *str == '\0') {
        return str;
    }
    str[strcspn(str, "\n")] = '\0';
    char* start = str;
    char* end = str + strlen(str);

    while (isspace(*start)) {
        start++;
    }

    while (end > start && isspace(*end)) {
        end--;
    }

    size_t len = end - start;
    char* final_str = (char*)malloc(len + 1);
    strncpy(final_str, start, len);
    final_str[len + 1] = '\0';

    return final_str;
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
    char** values = (char**)malloc((count + 1) * sizeof(char*));
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

void initialize_black_list(const char* filename, char** black_list) {

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "File %s could not open\n", filename);
        return;
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), file)) {
        if (*black_list == NULL) {
            *black_list = get_list("Domains", buffer);
        }

        if (black_list != NULL) {
            break;
        }
    }

    fclose(file);
}

int in_list(const char* target, char** list) {

    for (size_t i = 0; list[i] != NULL; ++i) {
        if (strcmp(target, list[i]) == 0) {
            return 1;
        }
    }

    return 0;
}
