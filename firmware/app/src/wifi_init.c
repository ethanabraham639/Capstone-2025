#include "wifi_init.h"
#include "wifi_handlers.h"

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
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sys.h"


static const char *TAG="WIFI_INIT.C";

/* WiFi configuration */
#define EXAMPLE_ESP_WIFI_SSID      "PuttPilot"
#define EXAMPLE_ESP_WIFI_PASS      "puttpilot"
#define EXAMPLE_MAX_STA_CONN       5

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

httpd_uri_t clear_sequence = {
    .uri       = "/clear_sequence",
    .method    = HTTP_POST,
    .handler   = POST_clearSequence_handler,
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

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

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
        httpd_register_uri_handler(server, &clear_sequence);
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

void wifi_init_softap()
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}

void WIFI_init_and_start_server(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_init_softap();

    httpd_handle_t server = start_webserver();
    if (server == NULL) {
        ESP_LOGE(TAG, "Failed to start the web server");
    }
}