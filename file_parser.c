#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_parser.h"

#define BUFFER_SIZE 2048

/**
 * Parses a line of text based on a given key and returns a vector of strings.
 * Each string represents a value associated with the key separated by commas.
 *
 * @param key The key to look for in the line.
 * @param line string (an array of chars) of text to parse.
 * @return Array of char* pointers containing the parsed values.
 */
static char** get_list(char* line);

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

static int count_size(const char* line) {
    int count = 0;
    int found_symbol = 0;

    // Iterate through the characters in the line
    while (*line != '\0') {
        if (*line == '=') {
            found_symbol = 1;
            if (isalnum(*(line + 1)) || *(line + 1) == ' ') {
                // add first word after '='
                count++;
            }
        }
        if (found_symbol && *line == ',') {
            // Check if the next character is not a comma or end of string
            if (*(line + 1) != ',' && *(line + 1) != '\0') {
                count++; // Found a non-empty word after the symbol, increment count
            }
        }
        line++;
    }
    return count;
}

static char** get_list(char* line) {

    int count = count_size(line);
    char** values = (char**)malloc((count + 1) * sizeof(char*));
    if (values == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    // reset count to parse
    count = 0;
    // cut off 'head' before '='
    char* token = strtok(strdup(line), "=");
    token = strtok(NULL, ",");
    // Continue tokenizing the remaining part of the line using comma as a delimiter
    while (token) {
        values[count] = trim_whitespaces(token);
        token = strtok(NULL, ",");
        count++;
    }

    values[count++] = NULL;

    return values;

}

char* initialize_upstream(const char* filename) {

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "File %s could not open\n", filename);
        return NULL;
    }

    char buffer[BUFFER_SIZE];
    const char* dom_key = "Upstream";
    while (fgets(buffer, sizeof(buffer), file)) {
       if (strncmp(buffer, dom_key , strlen(dom_key)) == 0) {
            char* start = strchr(buffer, '=');
            start++;
            return trim_whitespaces(start);
       }
    }

    fclose(file);
}

char** initialize_black_list(const char* filename) {

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "File %s could not open\n", filename);
        return NULL;
    }

    char buffer[BUFFER_SIZE];
    const char* dom_key = "Domains";
    while (fgets(buffer, sizeof(buffer), file)) {
       if (strncmp(buffer, dom_key , strlen(dom_key)) == 0) {
            return get_list(buffer);
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
