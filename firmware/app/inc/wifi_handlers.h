#ifndef WIFI_HANDLERS_H
#define WIFI_HANDLERS_H

#include "esp_wifi.h"
#include "esp_http_server.h"

esp_err_t POST_actuatorPositionHandler(httpd_req_t *req);
esp_err_t POST_actuatorResetHandler(httpd_req_t *req);
esp_err_t GET_debugMsgHandler(httpd_req_t *req);
esp_err_t echo_post_handler(httpd_req_t *req);

#endif