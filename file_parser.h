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
/**
 * @brief Checks if a target string is in a list of strings.
 *
 * This function iterates through the given list of strings until it encounters
 * a NULL pointer, indicating the end of the list. It compares the target string
 * with each string in the list using strcmp function.
 *
 * @param target The string to search for.
 * @param list   A null-terminated list of strings.
 *
 * @return 1 if the target string is found in the list, 0 otherwise.
 */
int in_list(const char* target, char** list);
#endif // FILE_PARSER_H
