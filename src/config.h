#if !defined(CONFIG_H)
#define CONFIG_H

#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef struct config {
  char export_path[PATH_MAX];
} config_t;

int config_parse(int argc, char **argv, config_t *config);

#endif  // CONFIG_H
