#ifndef FILE_PARSER_H
#define FILE_PARSER_H

/**
 * Parses a line of text based on a given key and returns a vector of strings.
 * Each string represents a value associated with the key separated by commas.
 *
 * @param key The key to look for in the line.
 * @param line string (an array of chars) of text to parse.
 * @return Array of char* pointers containing the parsed values.
 */
char** get_list(const char* key, char* line);
/**
 * Parses a line of text based on a given key and returns a string.
 *
 * @param key The key to look for in the line.
 * @param line string (an array of chars) of text to parse.
 * @return A string containing the parsed value.
 */
char* get_string(const char* key, char* line);
/**
 * Combine get_list and get_string function to initialize variables from .ini
 * file.
 *
 * @param filename File from which to read
 * @param black_list Vector containting forbidden addresses
 * @param upstream String containting address of upstream DNS server
 * */
void initialize(const char* filename, char** black_list, char* upstream);
#endif // FILE_PARSER_H
