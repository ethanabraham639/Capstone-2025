#include "wifi_handlers.h"
#include "actuator_control.h"
#include "common.h"

#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "esp_httpd_priv.h"

#define TAG "WIFI_SERVER"

esp_err_t POST_actuatorPositionHandler(httpd_req_t *req)
{
    char buffer[NUM_ACTUATORS] = {0};
    
    // Make sure the data length is what we expect
    int total_len = req->content_len;
    if (total_len != NUM_ACTUATORS) {
        ESP_LOGE(TAG, "Invalid data length: %d bytes (expected %d)", total_len, NUM_ACTUATORS);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST);
        return ESP_FAIL;
    }

    // Populate the buffer with the payload
    int received = httpd_req_recv(req, buffer, sizeof(buffer));

    // Make sure the received data is the size we expect
    if (received <= 0) {
        ESP_LOGE(TAG, "Failed to receive data");
        httpd_resp_send_err(req, HTTPD_500_SERVER_ERROR);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Received 45 bytes of servo data");

    // Update the motor positions
    updateActuatorPositions(buffer);

    return ESP_OK;
}

esp_err_t POST_actuatorResetHandler(httpd_req_t *req)
{    
    // Confirm no data is attached to this get request, send error otherwise
    if (req->content_len == 0)
    {
        ESP_LOGI(TAG, "Received POST request with no payload data");
        httpd_resp_send(req, NULL, 0);
    } 
    else 
    {
        ESP_LOGI(TAG, "Received POST request with data");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST);
    }

    // Update the motor positions
    resetAcuatorPositions();

    return ESP_OK;
}

esp_err_t GET_debugMsgHandler(httpd_req_t *req)
{
    // For now send a todo message
    // This should call a getter function to fetch the current debug message payload
    const char* resp_str = "TODO: Send real debug message";
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "GET_debugMsgHandler headers lost");
    }

    return ESP_OK;
}