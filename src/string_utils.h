#if !defined(STRING_UTILS_H)
#define STRING_UTILS_H

#include <slurm/slurm.h>

char* strdup(const char* s);
uint64_t find_tres_count_in_string(char* tres_str_in, int id);

#endif  // STRING_UTILS_H
