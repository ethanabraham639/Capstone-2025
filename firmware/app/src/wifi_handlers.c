#include "wifi_handlers.h"
#include "actuator_control.h"

#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"


/**
 * 
 * -this file will house the definitions for all the POST and GET handlers
 * -it will include any other files that provide an interface to update or get data for these handlers
 *      -so that means this file should not store any specific data
 * 
 * -in the case of the POST request with the motor positions
 *      - a post request handler will be defined that will call a setter fuction from actuator_control
 *        which will update the array holding the servo positions
 */

/* Handler for the /servo_motor_positions endpoint */
esp_err_t servo_motor_positions_handler(httpd_req_t *req) {
    // Buffer to hold the 45-byte data
    char buffer[NUM_ACTUATORS];
    
    size_t total_len = req->content_len;
    if (total_len != NUM_ACTUATORS) {
        ESP_LOGE(TAG, "Invalid data length: %d bytes (expected 45)", total_len);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid data length");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Received 45 bytes of servo data");

    // Process the servo data (example: logging it)
    for (int i = 0; i < 45; i++) {
        ESP_LOGI(TAG, "Byte %d: %d", i, buffer[i]);
    }

    // Send a response to the client
    httpd_resp_send(req, "Data received successfully", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
