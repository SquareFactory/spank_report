#include "spank_report.h"

#include <slurm/spank.h>
#include <stdlib.h>
#include <string.h>

#include "api.h"
#include "config.h"
#include "slurm_utils.h"

const char plugin_name[] = "Job SPANK report plugin";
const char plugin_type[] = "spank/report";

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
  // Parse argv
  config_t config = {.export_path = {0}};
  if (config_parse(ac, argv, &config) != 0) {
    slurm_error("%s: failed to parse configuration", plugin_type);
    return SLURM_ERROR;
  }

  // Fetch the job
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
  report_t report;
  parse_slurm_job_info(job, &report);

  // Output
  if (publish(&report, config.export_path) != 0) {
    slurm_error("%s: failed to publish the report", plugin_type);
  }

  free_report_members(&report);

  return SLURM_SUCCESS;
}

void free_report_members(report_t *report) {
  if (report->cluster) free(report->cluster);
  if (report->qos_name) free(report->qos_name);
  if (report->partition) free(report->partition);
}
