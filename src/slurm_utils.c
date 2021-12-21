#include "slurm_utils.h"

#include <slurm/slurmdb.h>
#include <slurm/spank.h>
#include <string.h>

#include "string_utils.h"

double fetch_usage_factor(char *qos_name) {
  double usage_factor = 0.0l;
  assoc_mgr_info_request_msg_t req;
  assoc_mgr_info_msg_t *msg = NULL;
  memset(&req, 0, sizeof(assoc_mgr_info_request_msg_t));
  req.flags = ASSOC_MGR_INFO_FLAG_QOS;
  if (slurm_load_assoc_mgr_info(&req, &msg) != SLURM_SUCCESS || msg == NULL) {
    goto finally;
  }

  ListIterator itr = slurm_list_iterator_create(msg->qos_list);
  slurmdb_qos_rec_t *qos;
  while ((qos = slurm_list_next(itr))) {
    if (qos->name != NULL && strncmp(qos->name, qos_name, BUFSIZ) == 0) {
      usage_factor = qos->usage_factor;
      break;
    }
  }
  slurm_list_iterator_destroy(itr);

finally:
  slurm_free_assoc_mgr_info_msg(msg);
  slurm_free_assoc_mgr_info_request_members(&req);
  return usage_factor;
}

void parse_slurm_job_info(slurm_job_info_t *job, report_t *report) {
  // Report the job factors to the server
  report->job_id = job->job_id;
  slurm_debug("%s: report->job_id %u", plugin_type, report->job_id);
  report->user_id = job->user_id;
  slurm_debug("%s: report->user_id %u", plugin_type, report->user_id);
  report->start_time = job->start_time;
  slurm_debug("%s: report->start_time %ld", plugin_type, report->start_time);

  if (job->cluster && job->cluster[0])
    report->cluster = strdup(job->cluster);
  else
    report->cluster = NULL;
  slurm_debug("%s: report->cluster %s", plugin_type, report->cluster);

  if (job->partition && job->partition[0])
    report->partition = strdup(job->partition);
  else
    report->partition = NULL;
  slurm_debug("%s: report->partition %s", plugin_type, report->partition);

  if (job->qos && job->qos[0])
    report->qos_name = strdup(job->qos);
  else
    report->qos_name = NULL;
  slurm_debug("%s: report->qos_name %s", plugin_type, report->qos_name);

  if (job->tres_alloc_str && job->tres_alloc_str[0]) {
    report->billing = (uint64_t)job->billable_tres;
  }
  slurm_debug("%s: report->billing %lu", plugin_type, report->billing);

  // Find the usage factor
  report->usage_factor = fetch_usage_factor(report->qos_name);
  slurm_debug("%s: report->usage_factor %lf", plugin_type,
              report->usage_factor);
}
