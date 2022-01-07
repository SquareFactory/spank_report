#include "string_utils.h"

#include <stdlib.h>
#include <string.h>

char* alt_strdup(const char* s) {
  size_t slen = strlen(s);
  char* result = malloc(slen + 1);
  if (result == NULL) {
    return NULL;
  }

  memcpy(result, s, slen + 1);
  return result;
}
