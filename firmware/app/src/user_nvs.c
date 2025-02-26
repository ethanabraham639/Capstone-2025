#include "user_nvs.h"
#include "actuator_control.h"
#include "error_codes.h"

#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_system.h"
#include <stdbool.h>
#include <sys/param.h>

#define NVS_APP_NAMESPACE "storage"
#define NVS_COURSE_STATE_KEY "course_state"

#define TAG "USER_NVS.C"

void NVS_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase(); // Erase and reinitialize
        err = nvs_flash_init();

        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "NVS init failed, cannot save anything");
            ERRORCODE_set(NVS_ERROR);
        }
    }
}

//save
esp_err_t NVS_write_course_state(uint8_t courseState[NUM_ACTUATORS])
{
    nvs_handle_t nvs_handle;

    esp_err_t err = nvs_open(NVS_APP_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%d) opening NVS handle to write course state", err);
        ERRORCODE_set(NVS_ERROR);

        return err;
    }

    err = nvs_set_blob(nvs_handle, NVS_COURSE_STATE_KEY, courseState, NUM_ACTUATORS);
    if (err == ESP_OK)
    {
        nvs_commit(nvs_handle); // Ensure data is saved
        ESP_LOGI(TAG, "Successfully saved course state to NVS");
    }
    else {
        ESP_LOGE(TAG, "Failed to save course state to NVS w/ error code (%d)", err);
        ERRORCODE_set(NVS_ERROR);

        return err; 
    }

    // Close NVS handle
    nvs_close(nvs_handle);

    return ESP_OK;
}

//read
esp_err_t NVS_read_course_state(uint8_t output[NUM_ACTUATORS])
{
    nvs_handle_t nvs_handle;

    esp_err_t err = nvs_open(NVS_APP_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%d) opening NVS handle to read course state", err);
        ERRORCODE_set(NVS_ERROR);

        return err;
    }
    
    // Get size of stored data
    size_t required_size = 0;
    err = nvs_get_blob(nvs_handle, NVS_COURSE_STATE_KEY, NULL, &required_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read course from NVS");
        ERRORCODE_set(NVS_ERROR);

        return ESP_FAIL;
    }

    if (required_size != NUM_ACTUATORS)
    {
        ESP_LOGE(TAG, "Failed to read course from NVS, mismatched data length, expected (%d), got (%d)", NUM_ACTUATORS, required_size);
        ERRORCODE_set(NVS_ERROR);
        
        return ESP_FAIL;
    }

    // Read the data
    err = nvs_get_blob(nvs_handle, NVS_COURSE_STATE_KEY, output, &required_size);
    if (err == ESP_OK) {

        char log_buffer[205];
        int pos = 0;
        for (uint8_t i = 0; i < NUM_ACTUATORS; i++) {
            pos += snprintf(log_buffer + pos, sizeof(log_buffer) - pos, "%d ", output[i]);
            if (pos >= sizeof(log_buffer)) {
                break; // Prevent buffer overflow
            }
        }

        ESP_LOGI(TAG, "Successfully read course state from NVS: %s", log_buffer);
    }

    // Close NVS handle
    nvs_close(nvs_handle);

    return ESP_OK;
}