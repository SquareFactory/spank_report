#include "string_utils.h"

#include <slurm/slurm.h>
#include <slurm/spank.h>
#include <stdlib.h>
#include <string.h>

#define slurm_atoull(str) strtoull(str, NULL, 10)

char* strdup(const char* s) {
  size_t slen = strlen(s);
  char* result = malloc(slen + 1);
  if (result == NULL) {
    return NULL;
  }

  memcpy(result, s, slen + 1);
  return result;
}

uint64_t find_tres_count_in_string(char* tres_str_in, int id) {
  char* tmp_str = tres_str_in;

  if (!tmp_str || !tmp_str[0]) return INFINITE64;

  while (tmp_str) {
    if (id == atoi(tmp_str)) {
      if (!(tmp_str = strchr(tmp_str, '='))) {
        slurm_error(
            "slurmdb_find_tres_count_in_string: "
            "no value found");
        break;
      }
      return slurm_atoull(++tmp_str);
    }

    if (!(tmp_str = strchr(tmp_str, ','))) break;
    tmp_str++;
  }

  return INFINITE64;
}
