#if !defined(API_H)
#define API_H

#include "spank_report.h"

/**
 * @brief Publish the report in a file stored in the log directory.
 *
 * @param report Report structure to be export.
 * @param export_path Directory UNIX path.
 * @return int Error code.
 */
int publish(report_t* report, char* export_path);

#endif  // API_H
