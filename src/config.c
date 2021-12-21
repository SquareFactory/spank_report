#include "config.h"

#include <slurm/spank.h>
#include <stdlib.h>

#include "spank_report.h"

int config_parse(int argc, char **argv, config_t *config) {
  const char *optarg;

  memset(config, 0, sizeof(*config));

  strcpy(config->export_path, "/tmp");

  for (int i = 0; i < argc; i++) {
    if (strncmp("export_path=", argv[i], 13) == 0) {
      optarg = argv[i] + 13;
      if (memccpy(config->export_path, optarg, '\0',
                  sizeof(config->export_path)) == NULL) {
        slurm_error("%s: export_path: path too long: %s", plugin_type, optarg);
        return -1;
      }
    } else {
      slurm_error("%s: unknown configuration option: %s", plugin_type, argv[i]);
      return -1;
    }
  }

  return 0;
}
