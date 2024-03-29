#if !defined(SPANK_REPORT_H)
#define SPANK_REPORT_H

#include <inttypes.h>
#include <time.h>

extern const char plugin_name[];
extern const char plugin_type[];
extern const uint32_t plugin_version;

typedef struct report {
  /** @brief A Slurm Job ID. */
  uint32_t job_id;
  /** @brief A UNIX user ID. */
  uint32_t user_id;
  /** @brief A Slurm Cluster name. */
  char *cluster;
  /** @brief A Slurm Partition name. */
  char *partition;
  /** @brief The billing tres factor. */
  uint64_t billing;
  /** @brief The job start timestamp. */
  time_t start_time;
  /** @brief The name of the Qos. */
  char *qos_name;
  /** @brief The usage factor of the Qos. */
  double usage_factor;
  /** @brief The priority of the job. */
  uint32_t priority;
} report_t;

void free_report_members(report_t *report);

#endif  // SPANK_REPORT_H
