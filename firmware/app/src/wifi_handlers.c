#include "wifi_handlers.h"
#include "actuator_control.h"

#include <sys/param.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"

#define TAG "WIFI_HANDLERS.C"

#define COURSE_STATE_POST_REQ_SIZE      (NUM_ACTUATORS + MODES_SIZE)
#define RESET_STATS_POST_REQ_SIZE       1
#define DISPENSE_BALLS_POST_REQ_SIZE    1

esp_err_t POST_courseState_handler(httpd_req_t *req)
{
    char buffer[COURSE_STATE_POST_REQ_SIZE] = {0};
    
    // Make sure the data length is what we expect
    int total_len = req->content_len;
    if (total_len != (COURSE_STATE_POST_REQ_SIZE)) {
        ESP_LOGE(TAG, "Invalid data length POST_courseState_handler: %d bytes (expected %d)", total_len, NUM_ACTUATORS);
        return ESP_FAIL;
    }

    // Populate the buffer with the payload
    int received = httpd_req_recv(req, buffer, sizeof(buffer));

    // Make sure the received data is the size we expect
    if (received <= 0) {
        ESP_LOGE(TAG, "Failed to receive data");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Received 45 bytes of servo data");

    // Update the motor positions
    // AC_updateMode((uint8_t)buffer[0]);
    // AC_updatePositions(&buffer[1]);

    return ESP_OK;
}

esp_err_t POST_resetStats_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "resetStats POST request received.");

    // BE_resetStats();

    return ESP_OK;
}

esp_err_t POST_settings_handler(httpd_req_t *req)
{
    char buffer[RESET_STATS_POST_REQ_SIZE] = {0};
    
    // Make sure the data length is what we expect
    int total_len = req->content_len;
    if (total_len != (RESET_STATS_POST_REQ_SIZE)) {
        ESP_LOGE(TAG, "Invalid data length in POST_settings_handler: %d bytes (expected %d)", total_len, RESET_STATS_POST_REQ_SIZE);
        return ESP_FAIL;
    }

    // Populate the buffer with the payload
    int received = httpd_req_recv(req, buffer, sizeof(buffer));

    // Make sure the received data is the size we expect
    if (received <= 0) {
        ESP_LOGE(TAG, "Failed to receive data");
        return ESP_FAIL;
    }

    // update_auto_dispense((uint8_t) buffer[0]);

    return ESP_OK;
}

esp_err_t POST_dispenseBall_handler(httpd_req_t *req)
{
    char buffer[DISPENSE_BALLS_POST_REQ_SIZE] = {0};
    
    // Make sure the data length is what we expect
    int total_len = req->content_len;
    if (total_len != (DISPENSE_BALLS_POST_REQ_SIZE)) {
        ESP_LOGE(TAG, "Invalid data length in POST_dispenseBall_handler: %d bytes (expected %d)", total_len, RESET_STATS_POST_REQ_SIZE);
        return ESP_FAIL;
    }

    // Populate the buffer with the payload
    int received = httpd_req_recv(req, buffer, sizeof(buffer));

    // Make sure the received data is the size we expect
    if (received <= 0) {
        ESP_LOGE(TAG, "Failed to receive data");
        return ESP_FAIL;
    }

    // BQ_dispense_balls((uint8_t)buffer[0]);

    return ESP_OK;
}


esp_err_t GET_errorCodes_handler(httpd_req_t *req)
{
    return ESP_OK;
}

esp_err_t GET_debugMsg_handler(httpd_req_t *req)
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

esp_err_t GET_stats_handler(httpd_req_t *req)
{
    uint8_t ballsHit = BE_get_balls_hit();
    uint8_t ballsInHole = BE_get_balls_in_hole();

    const char resp_str[2] = {ballsHit, ballsInHole};
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "GET_stats_handler headers lost");
    }

    return ESP_OK;
}





// Temp handler for testing purposes
esp_err_t echo_post_handler(httpd_req_t *req)
{
    char buf[1000];
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");
    }

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}