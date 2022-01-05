#include "api.h"

#include <curl/curl.h>
#include <slurm/spank.h>
#include <stdio.h>
#include <sys/stat.h>

#define POSTDATA_FORMAT                                           \
  "{\"properties\": {}, \"routing_key\": \"%s\", \"payload\": \"" \
  "{\\\"job_id\\\": %u, "                                         \
  "\\\"user_id\\\": %u, "                                         \
  "\\\"cluster\\\": \\\"%s\\\", "                                 \
  "\\\"partition\\\": \\\"%s\\\", "                               \
  "\\\"billing\\\": %lu, "                                        \
  "\\\"start_time\\\": %ld, "                                     \
  "\\\"qos_name\\\": \\\"%s\\\", "                                \
  "\\\"usage_factor\\\": %lf "                                    \
  "}\", \"payload_encoding\": \"string\"}"

int publish(report_t* report, char* export_path, char* routing_key, char* user,
            char* password) {
  int rc = 0;
  CURL* curl;
  CURLcode res;
  curl = curl_easy_init();
  if (curl) {
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, export_path);

    char body[BUFSIZ];
    snprintf(body, sizeof(body), POSTDATA_FORMAT, routing_key, report->job_id,
             report->user_id, report->cluster, report->partition,
             report->billing, report->start_time, report->qos_name,
             report->usage_factor);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(curl, CURLOPT_USERNAME, user);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      slurm_error("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      rc = 1;
    }

    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  return rc;
}
