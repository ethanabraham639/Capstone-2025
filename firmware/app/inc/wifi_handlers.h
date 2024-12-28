#ifndef WIFI_HANDLERS_H
#define WIFI_HANDLERS_H

#include "esp_wifi.h"
#include "esp_http_server.h"

// POST handlers
esp_err_t POST_courseState_handler(httpd_req_t *req);
esp_err_t POST_resetStats_handler(httpd_req_t *req);
esp_err_t POST_settings_handler(httpd_req_t *req);
esp_err_t POST_dispenseBall_handler(httpd_req_t *req);

// GET handlers
esp_err_t GET_errorCodes_handler(httpd_req_t *req);
esp_err_t GET_debugMsg_handler(httpd_req_t *req);
esp_err_t GET_stats_handler(httpd_req_t *req);

// Temp handler for testing purposes
esp_err_t echo_post_handler(httpd_req_t *req);

#endif