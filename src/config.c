#include "config.h"

#include <slurm/spank.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spank_report.h"

int config_parse(int argc, char **argv, config_t *config) {
  const char *optarg;

  memset(config, 0, sizeof(*config));

  strcpy(config->export_path, "/tmp");

  for (int i = 0; i < argc; i++) {
    if (strncmp("export_path=", argv[i], 13) == 0) {
      optarg = argv[i] + 13;
      snprintf(config->export_path, sizeof(config->export_path), "%s", optarg);
    } else {
      slurm_error("%s: unknown configuration option: %s", plugin_type, argv[i]);
      return -1;
    }
  }

  return 0;
}
