#include <slurm/slurm.h>
#include <slurm/slurmdb.h>
#include <slurm/spank.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_utils.h"

#define TRES_BILLING 5

/**
 * @brief Returns the value of TRES. Otherwise, returns INFINITE64.
 *
 * @param tres_str_in TRES string format.
 * @param id ID of the TRES.
 * @return uint64_t Value of the TRES.
 */
uint64_t find_tres_count_in_string(char *tres_str_in, int id);

const char plugin_name[] = "Job SPANK report plugin";
const char plugin_type[] = "spank/report";

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
  "job_id: %u\n"               \
  "user_id: %u\n"              \
  "cluster: %s\n"              \
  "partition: %s\n"            \
  "billable_ressources: %lu\n" \
  "time_start: %ld\n"          \
  "cost_tier:\n"               \
  "  name: %s\n"               \
  "  factor: %lf\n"

/**
 * @brief Called in local (srun) context only after all options have been
 * processed. This is called after the job ID and step IDs are available. This
 * happens in srun after the allocation is made, but before tasks are launched.
 *
 * @param spank (input) SPANK handle which must be passed back to Slurm when the
 * plugin calls functions like spank_get_item and spank_getenv.
 * @param ac Argument count
 * @param argv Argument vector
 * @return int Error code
 */
extern int slurm_spank_local_user_init(spank_t spank, int ac, char *argv[]) {
  unsigned int jobid = 0;
  job_info_msg_t *job_info = NULL;
  if (spank_get_item(spank, S_JOB_ID, &jobid) != 0) {
    slurm_error("%s: couldn't find the job ID", plugin_type);
    return SLURM_SUCCESS;
  }
  slurm_debug("%s: start %s %d", plugin_type, __func__, jobid);
  if (slurm_load_job(&job_info, jobid, SHOW_ALL) != 0) {
    slurm_error("%s: couldn't load the job %u", plugin_type, jobid);
    return SLURM_SUCCESS;
  }

  slurm_job_info_t *job = job_info->job_array;

  // Report the job factors to the server
  report_t report = {.job_id = job->job_id,
                     .user_id = job->user_id,
                     .start_time = job->start_time};

  if (job->cluster && job->cluster[0])
    report.cluster = strdup(job->cluster);
  else
    report.cluster = NULL;
  slurm_debug("%s: report.cluster %s", plugin_type, report.cluster);

  if (job->partition && job->partition[0])
    report.partition = strdup(job->partition);
  else
    report.partition = NULL;
  slurm_debug("%s: report.partition %s", plugin_type, report.partition);

  if (job->qos && job->qos[0])
    report.qos_name = strdup(job->qos);
  else
    report.qos_name = NULL;
  slurm_debug("%s: report.qos %s", plugin_type, report.qos_name);

  if (job->tres_alloc_str && job->tres_alloc_str[0]) {
    slurm_debug("%s: job tres %s", plugin_type, job->tres_alloc_str);
    report.billing =
        find_tres_count_in_string(job->tres_alloc_str, TRES_BILLING);
  }
  slurm_debug("%s: report.billing %lu", plugin_type, report.billing);

  // Find the usage factor
  assoc_mgr_info_request_msg_t req;
  assoc_mgr_info_msg_t *msg = NULL;
  memset(&req, 0, sizeof(assoc_mgr_info_request_msg_t));
  req.flags = ASSOC_MGR_INFO_FLAG_QOS;
  if (slurm_load_assoc_mgr_info(&req, &msg) != SLURM_SUCCESS || msg == NULL) {
    goto cleanup;
  }

  ListIterator itr = slurm_list_iterator_create(msg->qos_list);
  slurmdb_qos_rec_t *qos;
  while ((qos = slurm_list_next(itr))) {
    slurm_debug("%s: test qos %s", plugin_type, qos->name);
    if (qos->name != NULL && strncmp(qos->name, report.qos_name, 20) == 0) {
      report.usage_factor = qos->usage_factor;
      break;
    }
  }
  slurm_list_iterator_destroy(itr);
  slurm_debug("%s: report.usage_factor %lf", plugin_type, report.usage_factor);

  // Output
  char log_path[1024];
  snprintf(log_path, sizeof(log_path), "%s/%u.report", "/tmp", job->job_id);

  slurm_debug("%s: archiving %s", plugin_type, log_path);

  char buffer[1024];
  snprintf(buffer, sizeof(buffer), REPORT_FORMAT, report.job_id, report.user_id,
           report.cluster, report.partition, report.billing, report.start_time,
           report.qos_name, report.usage_factor);

  slurm_debug("%s\n", buffer);

  FILE *output = fopen(log_path, "w");
  if (output != NULL) {
    fprintf(output, "%s\n", buffer);
    fclose(output);
  } else {
    goto cleanup;
  }

cleanup:
  slurm_free_assoc_mgr_info_msg(msg);
  slurm_free_assoc_mgr_info_request_members(&req);
  free(report.cluster);
  free(report.qos_name);
  free(report.partition);

  return SLURM_SUCCESS;
}
