#include "config.h"

#include <slurm/spank.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spank_report.h"

int config_parse(int argc, char **argv, config_t *config) {
  const char *optarg;

  memset(config, 0, sizeof(*config));

  strcpy(config->rmq_api_url,
         "http://localhost:15672/api/exchanges/%2F/amq.default/publish");
  strcpy(config->username, "guest");
  strcpy(config->password, "guest");
  strcpy(config->routing_key, "job_report");

  for (int i = 0; i < argc; i++) {
    if (strncmp("rmq_api_url=", argv[i], 12) == 0) {
      optarg = argv[i] + 12;
      snprintf(config->rmq_api_url, sizeof(config->rmq_api_url), "%s", optarg);
    } else if (strncmp("username=", argv[i], 9) == 0) {
      optarg = argv[i] + 9;
      snprintf(config->username, sizeof(config->username), "%s", optarg);
    } else if (strncmp("password=", argv[i], 9) == 0) {
      optarg = argv[i] + 9;
      snprintf(config->password, sizeof(config->password), "%s", optarg);
    } else if (strncmp("routing_key=", argv[i], 12) == 0) {
      optarg = argv[i] + 12;
      snprintf(config->routing_key, sizeof(config->routing_key), "%s", optarg);
    } else {
      slurm_error("%s: unknown configuration option: %s", plugin_type, argv[i]);
      return -1;
    }
  }

  return 0;
}
