#include "wifi_init.h"
#include "wifi_handlers.h"
#include "wifi_connect.h"

#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"

#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs.h"
#include "nvs_flash.h"

#include <esp_http_server.h>

static const char *TAG="WIFI_INIT.C";


httpd_uri_t course_state = {
    .uri       = "/course_state",
    .method    = HTTP_POST,
    .handler   = POST_courseState_handler,
    .user_ctx  = NULL
};

httpd_uri_t reset_stats = {
    .uri       = "/reset_stats",
    .method    = HTTP_POST,
    .handler   = POST_resetStats_handler,
    .user_ctx  = NULL
};

httpd_uri_t settings = {
    .uri       = "/settings",
    .method    = HTTP_POST,
    .handler   = POST_settings_handler,
    .user_ctx  = NULL
};

httpd_uri_t dispense_ball = {
    .uri       = "/dispense_ball",
    .method    = HTTP_POST,
    .handler   = POST_dispenseBall_handler,
    .user_ctx  = NULL
};

httpd_uri_t echo = {
    .uri       = "/echo",
    .method    = HTTP_POST,
    .handler   = echo_post_handler,
    .user_ctx  = NULL
};


httpd_uri_t error_codes = {
    .uri       = "/error_codes",
    .method    = HTTP_GET,
    .handler   = GET_errorCodes_handler,
    .user_ctx  = NULL
};

httpd_uri_t debug_msg = {
    .uri       = "/debug_msg",
    .method    = HTTP_GET,
    .handler   = GET_debugMsg_handler,
    .user_ctx  = NULL
};

httpd_uri_t stats = {
    .uri       = "/stats",
    .method    = HTTP_GET,
    .handler   = GET_stats_handler,
    .user_ctx  = NULL
};

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &course_state);
        httpd_register_uri_handler(server, &reset_stats);
        httpd_register_uri_handler(server, &settings);
        httpd_register_uri_handler(server, &dispense_ball);
        httpd_register_uri_handler(server, &error_codes);
        httpd_register_uri_handler(server, &debug_msg);
        httpd_register_uri_handler(server, &stats);

        httpd_register_uri_handler(server, &echo);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static httpd_handle_t server = NULL;

static void disconnect_handler(void* arg, esp_event_base_t event_base, 
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base, 
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

void WIFI_init_and_start_server(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());


    ESP_ERROR_CHECK(example_connect());

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    server = start_webserver();
}