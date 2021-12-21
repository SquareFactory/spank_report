#if !defined(SLURM_UTILS_H)
#define SLURM_UTILS_H

#include <slurm/slurm.h>

#include "spank_report.h"

/**
 * @brief Fetch the usage factor from slurmdb
 *
 * @param qos_name (input) The Quality of Service name.
 * @return double The usage factor.
 */
double fetch_usage_factor(char* qos_name);

/**
 * @brief Parse the job info and put in the report struct.
 *
 * @param job (input) The slurm job info.
 * @param report (output) The report.
 * @return int Error code.
 */
void parse_slurm_job_info(slurm_job_info_t* job, report_t* report);

#endif  // SLURM_UTILS_H
