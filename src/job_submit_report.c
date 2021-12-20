#include <stdio.h>

#include "slurm/slurm_errno.h"
#include "src/common/slurm_xlator.h"
#include "src/slurmctld/locks.h"
#include "src/slurmctld/slurmctld.h"

const char plugin_name[] = "Job submit report plugin";
const char plugin_type[] = "job_submit/report";
const uint32_t plugin_version = SLURM_VERSION_NUMBER;

// TODO: fetch balance
#define MOCK_BALANCE 10000

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
} report_t;

#define REPORT_FORMAT          \
  "job_id: %d\n"               \
  "user_id: %d\n"              \
  "cluster: %s\n"              \
  "partition: %s\n"            \
  "billable_ressources: %ld\n" \
  "time_start: %ld\n"          \
  "cost_tier:\n"               \
  "  name: %s\n"               \
  "  factor: %lf\n"

/**
 * @brief Called when the plugin is loaded, before any other functions are
 * called. Put global initialization here.
 *
 * @return int SLURM_SUCCESS on success, or SLURM_ERROR on failure.
 */
extern int init(void) {
  slurm_info("%s: Initializing %s", plugin_type, plugin_name);
  return SLURM_SUCCESS;
}

/**
 * @brief Called when the plugin is removed. Clear any allocated storage here.
 *
 * @return int SLURM_SUCCESS on success, or SLURM_ERROR on failure.
 */
extern int fini(void) {
  slurm_info("%s: Finishing %s", plugin_type, plugin_name);
  return SLURM_SUCCESS;
}

/**
 * @brief This function is called by the slurmctld daemon with job submission
 * parameters supplied by the salloc, sbatch or srun command. It can be used to
 * log and/or modify the job parameters supplied by the user as desired. Note
 * that this function has access to the slurmctld's global data structures, for
 * example to examine the available partitions, reservations, etc.
 *
 * @param job_desc (input/output) the job allocation request specifications.
 * @param submit_uid (input) user ID initiating the request.
 * @param err_msg (output) If the argument is not null, then a plugin generated
 * error message can be stored here. The error message is expected to have
 * allocated memory which Slurm will release using the xfree function. The error
 * message is always propagated to the caller, no matter the return code.
 * @return SLURM_SUCCESS on success, or SLURM_ERROR on failure.
 */
extern int job_submit(job_desc_msg_t *job_desc, uint32_t submit_uid,
                      char **err_msg) {
  debug("%s: start %s %d", plugin_type, __func__, job_desc->job_id);
  int rc = SLURM_SUCCESS;

  // Report the job factors to the server
  report_t report = {.job_id = job_desc->job_id,
                     .user_id = job_desc->user_id,
                     .start_time = job_desc->begin_time};

  if (job_desc->origin_cluster && job_desc->origin_cluster[0])
    report.cluster = xstrdup(job_desc->origin_cluster);
  else
    report.cluster = NULL;
  debug("%s: report.cluster %s", plugin_type, report.cluster);

  if (job_desc->partition && job_desc->partition[0])
    report.partition = xstrdup(job_desc->partition);
  else
    report.partition = NULL;
  debug("%s: report.partition %s", plugin_type, report.partition);

  if (job_desc->qos && job_desc->qos[0])
    report.qos_name = xstrdup(job_desc->qos);
  else
    report.qos_name = NULL;
  debug("%s: report.qos %s", plugin_type, report.qos_name);

  if (job_desc->tres_req_cnt) {
    report.billing = job_desc->tres_req_cnt[TRES_ARRAY_BILLING];
  }
  debug("%s: report.billing %ld", plugin_type, report.billing);

  // Fetch slurm assoc mgr info to be able to fetch the usage factor
  assoc_mgr_info_request_msg_t req;
  assoc_mgr_info_msg_t *msg = NULL;
  memset(&req, 0, sizeof(assoc_mgr_info_request_msg_t));
  int cc = slurm_load_assoc_mgr_info(&req, &msg);
  if (cc != SLURM_SUCCESS) {
    slurm_perror("slurm_load_assoc_mgr_info error");
    rc = SLURM_ERROR;
    goto cleanup;
  }

  // Find the usage factor
  ListIterator itr = list_iterator_create(msg->qos_list);
  slurmdb_qos_rec_t *qos_ptr;
  while ((qos_ptr = list_next(itr))) {
    if (xstrcmp(qos_ptr->name, report.qos_name) == 0) {
      report.usage_factor = qos_ptr->usage_factor;
      break;
    }
  }
  list_iterator_destroy(itr);
  debug("%s: report.usage_factor %lf", plugin_type, report.usage_factor);

  // Output
  char log_path[1024];
  snprintf(log_path, sizeof(log_path), "%s/%d.report", "/tmp",
           job_desc->job_id);
  debug("%s: archiving %s", plugin_type, log_path);

  char buffer[1024];
  snprintf(buffer, sizeof(buffer), REPORT_FORMAT, report.job_id, report.user_id,
           report.cluster, report.partition, report.billing, report.start_time,
           report.qos_name, report.usage_factor);

  FILE *output = fopen(log_path, "w");
  if (output != NULL) {
    fprintf(output, "%s\n", buffer);
    fclose(output);
  } else {
    rc = SLURM_ERROR;
    goto cleanup;
  }

cleanup:
  slurm_free_assoc_mgr_info_msg(msg);
  xfree(report.cluster);
  xfree(report.qos_name);
  xfree(report.partition);

  return rc;
}

/**
 * @brief This function is called by the slurmctld daemon with job modification
 * parameters supplied by the scontrol or sview command. It can be used to log
 * and/or modify the job parameters supplied by the user as desired. Note that
 * this function has access to the slurmctld's global data structures, for
 * example to examine the available partitions, reservations, etc.
 *
 * @param job_desc (input/output) the job allocation request specifications.)
 * @param job_ptr  (input/output) slurmctld daemon's current data structure for
 * the job to be modified.
 * @param submit_uid (input) user ID initiating the request.
 * @return int SLURM_SUCCESS on success, or SLURM_ERROR on failure.
 */
extern int job_modify(job_desc_msg_t *job_desc, job_record_t *job_ptr,
                      uint32_t submit_uid) {
  return SLURM_SUCCESS;
}
