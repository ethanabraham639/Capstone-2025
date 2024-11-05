#include "wifi_init.h"
#include "wifi_handlers.h"

#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "esp_httpd_priv.h"


httpd_uri_t actuator_position_uri = {
    .uri      = "/actuator_position",
    .method   = HTTP_POST,
    .handler  = POST_actuatorPositionHandler,
    .user_ctx = NULL
};

httpd_uri_t actuator_reset_uri = {
    .uri      = "/actuator_reset",
    .method   = HTTP_POST,
    .handler  = POST_actuatorResetHandler,
    .user_ctx = NULL
};

httpd_uri_t debug_msg_uri = {
    .uri      = "/debug_msg",
    .method   = HTTP_GET,
    .handler  = GET_debugMsgHandler,
    .user_ctx = NULL
};