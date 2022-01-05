#if !defined(CONFIG_H)
#define CONFIG_H

#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef struct config {
  char rmq_api_url[PATH_MAX];
  char username[PATH_MAX];
  char password[PATH_MAX];
  char routing_key[PATH_MAX];
} config_t;

int config_parse(int argc, char **argv, config_t *config);

#endif  // CONFIG_H
