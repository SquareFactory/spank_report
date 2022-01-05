#include "config.h"

#include <slurm/spank.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spank_report.h"

int config_parse(int argc, char **argv, config_t *config) {
  const char *optarg;

  memset(config, 0, sizeof(*config));

  strcpy(config->export_path,
         "http://localhost:15672/api/exchanges/%2F/amq.default/publish");
  strcpy(config->username, "guest");
  strcpy(config->password, "guest");
  strcpy(config->routing_key, "job_report");

  for (int i = 0; i < argc; i++) {
    if (strncmp("export_path=", argv[i], 13) == 0) {
      optarg = argv[i] + 13;
      snprintf(config->export_path, sizeof(config->export_path), "%s", optarg);
    } else if (strncmp("username=", argv[i], 10) == 0) {
      optarg = argv[i] + 10;
      snprintf(config->username, sizeof(config->username), "%s", optarg);
    } else if (strncmp("password=", argv[i], 10) == 0) {
      optarg = argv[i] + 10;
      snprintf(config->password, sizeof(config->password), "%s", optarg);
    } else if (strncmp("routing_key=", argv[i], 13) == 0) {
      optarg = argv[i] + 13;
      snprintf(config->routing_key, sizeof(config->routing_key), "%s", optarg);
    } else {
      slurm_error("%s: unknown configuration option: %s", plugin_type, argv[i]);
      return -1;
    }
  }

  return 0;
}
