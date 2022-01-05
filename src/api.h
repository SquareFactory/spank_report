#if !defined(API_H)
#define API_H

#include "spank_report.h"

/**
 * @brief Publish the report in a RMQ using its API.
 *
 * @param report Report structure to be published.
 * @param url RabbitMQ API URL
 * @param routing_key Queue name
 * @param user A RMQ User.
 * @param password The password.
 * @return int Error code.
 */
int publish(report_t* report, char* url, char* routing_key, char* user,
            char* password);

#endif  // API_H
