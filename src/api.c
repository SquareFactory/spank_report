#include "api.h"

#include <slurm/spank.h>
#include <stdio.h>
#include <sys/stat.h>

#define REPORT_FORMAT          \
  "job_id: %u\n"               \
  "user_id: %u\n"              \
  "cluster: %s\n"              \
  "partition: %s\n"            \
  "billable_ressources: %lu\n" \
  "time_start: %ld\n"          \
  "cost_tier:\n"               \
  "  name: %s\n"               \
  "  factor: %lf\n"

int publish(report_t* report, char* export_path) {
  struct stat s;
  if (export_path == NULL || stat(export_path, &s) != 0 ||
      !S_ISDIR(s.st_mode)) {
    slurm_error("%s: directory '%s' is not accessible", plugin_type,
                export_path);
    return 1;
  }

  char log_path[BUFSIZ];
  snprintf(log_path, sizeof(log_path), "%s/%u.report", export_path,
           report->job_id);

  char buffer[BUFSIZ];
  snprintf(buffer, sizeof(buffer), REPORT_FORMAT, report->job_id,
           report->user_id, report->cluster, report->partition, report->billing,
           report->start_time, report->qos_name, report->usage_factor);

  FILE* output = fopen(log_path, "w");
  if (output != NULL) {
    fprintf(output, "%s\n", buffer);
    fclose(output);
    return 0;
  } else {
    slurm_error("%s: couldn't write '%s'", plugin_type, log_path);
    return 1;
  }
}
