#include "api.h"

#include <curl/curl.h>
#include <slurm/spank.h>
#include <stdio.h>
#include <stdlib.h>

#include "cJSON.h"

/**
 * @brief Add the fields from the report. output must be initialized.
 *
 * @param report The report object.
 * @param output A JSON object.
 * @param routing_key The RMQ queue.
 * @return int Error code.
 */
int build_json_object(const report_t* report, const char* routing_key,
                      cJSON* output);

int publish(report_t* report, char* export_path, char* routing_key, char* user,
            char* password) {
  int rc = 0;
  CURLcode res;
  cJSON* body = NULL;
  char* body_str = NULL;
  CURL* curl = curl_easy_init();
  if (!curl) {
    slurm_error("curl_easy_init failed");
    goto cleanup;
  }
  body = cJSON_CreateObject();
  if (!body) {
    slurm_error("cJSON_CreateObject: failed");
    rc = 1;
    goto cleanup;
  }

  struct curl_slist* headers = NULL;
  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_URL, export_path);
  curl_easy_setopt(curl, CURLOPT_USERNAME, user);
  curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

  if (build_json_object(report, routing_key, body) != 0) {
    slurm_error("build_json_object: failed");
    rc = 1;
    goto cleanup;
  }
  body_str = cJSON_PrintUnformatted(body);
  if (!body_str) {
    slurm_error("cJSON_PrintUnformatted: failed");
    rc = 1;
    goto cleanup;
  }
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_str);

  res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    slurm_error(
        "curl_easy_perform() failed: %s\n"
        "url was %s\n"
        "body was %s\n",
        curl_easy_strerror(res), export_path, body_str);
    rc = 1;
  }

cleanup:
  curl_easy_cleanup(curl);
  curl_global_cleanup();
  cJSON_Delete(body);
  free(body_str);
  return rc;
}

int build_json_object(const report_t* report, const char* routing_key,
                      cJSON* output) {
  char* payload_str = NULL;
  cJSON* payload = NULL;
  int rc = 0;
  if (!cJSON_AddObjectToObject(output, "properties")) {
    slurm_error("cJSON_AddObjectToObject: failed");
    return 1;
  }
  if (!cJSON_AddStringToObject(output, "routing_key", routing_key)) {
    slurm_error("cJSON_AddStringToObject: failed");
    return 1;
  }
  if (!cJSON_AddStringToObject(output, "payload_encoding", "string")) {
    slurm_error("cJSON_AddStringToObject: failed");
    return 1;
  }

  // Build payload
  payload = cJSON_CreateObject();
  if (!payload) {
    slurm_error("cJSON_CreateObject: failed");
    rc = 1;
    goto cleanup;
  }
  if (!cJSON_AddNumberToObject(payload, "job_id", report->job_id)) {
    slurm_error("cJSON_AddNumberToObject: failed");
    rc = 1;
    goto cleanup;
  }
  if (!cJSON_AddNumberToObject(payload, "user_id", report->user_id)) {
    slurm_error("cJSON_AddNumberToObject: failed");
    rc = 1;
    goto cleanup;
  }
  if (!cJSON_AddStringToObject(payload, "cluster", report->cluster)) {
    slurm_error("cJSON_AddStringToObject: failed");
    rc = 1;
    goto cleanup;
  }
  if (!cJSON_AddStringToObject(payload, "partition", report->partition)) {
    slurm_error("cJSON_AddStringToObject: failed");
    rc = 1;
    goto cleanup;
  }
  if (!cJSON_AddNumberToObject(payload, "billing", report->billing)) {
    slurm_error("cJSON_AddNumberToObject: failed");
    rc = 1;
    goto cleanup;
  }
  if (!cJSON_AddNumberToObject(payload, "start_time", report->start_time)) {
    slurm_error("cJSON_AddNumberToObject: failed");
    rc = 1;
    goto cleanup;
  }
  if (!cJSON_AddStringToObject(payload, "qos_name", report->qos_name)) {
    slurm_error("cJSON_AddStringToObject: failed");
    rc = 1;
    goto cleanup;
  }
  if (!cJSON_AddNumberToObject(payload, "usage_factor", report->usage_factor)) {
    slurm_error("cJSON_AddNumberToObject: failed");
    rc = 1;
    goto cleanup;
  }

  // Payload as string
  payload_str = cJSON_PrintUnformatted(payload);
  if (!payload_str) {
    slurm_error("cJSON_PrintUnformatted: failed");
    rc = 1;
    goto cleanup;
  }

  if (!cJSON_AddStringToObject(output, "payload", payload_str)) {
    slurm_error("cJSON_AddStringToObject: failed");
    rc = 1;
    goto cleanup;
  }

cleanup:
  cJSON_Delete(payload);
  free(payload_str);
  return rc;
}
