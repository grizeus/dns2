#ifndef FILE_PARSER_H
#define FILE_PARSER_H

/**
 * Combine get_list and get_string function to initialize variables from .ini
 * file.
 *
 * @param filename File from which to read
 * @param black_list Vector containting forbidden addresses
 * */
void initialize_black_list(const char* filename, char** black_list);
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
