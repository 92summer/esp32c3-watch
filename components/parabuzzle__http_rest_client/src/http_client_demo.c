#include <stdio.h>
#include <inttypes.h>
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "event_handlers.h"
#include "http_rest_json_client.h"
#include "cJSON.h"

#include "http_client_demo.h"
#define USE_USER_WIFI 0
#if USE_USER_WIFI
#define WIFI_SSID       "Mi10Pro"
#define WIFI_PASSWORD   "12345687"
#else
#define WIFI_SSID       "耶耶"
#define WIFI_PASSWORD   "12345687!"
#endif

// 实时天气
// #define URL "http://api.seniverse.com/v3/weather/now.json?key=STxpmSa4oTixXmbym&location=%E6%AD%A6%E6%B1%89&language=zh-Hans&unit=c"
// 天气预报
#define URL "http://api.seniverse.com/v3/weather/daily.json?key=STxpmSa4oTixXmbym&location=%E6%AD%A6%E6%B1%89&language=zh-Hans&unit=c&start=0&days=5"

static const char *TAG = "http rest client demo";

void WIFIConnection(void);
void TaskHTTP(void* param)
{
    WIFIConnection();
    esp_err_t ret = ESP_OK;
  // start main loop
    while (1)
    {
        // create a buffer to store the response
        http_rest_recv_json_t response_buffer = {0};

        // get the response from the server
        ESP_LOGI(TAG, "Fetching Data from URL: %s", URL);
        ret = http_rest_client_get_json(URL, &response_buffer);

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(ret));
            http_rest_client_cleanup_json(&response_buffer);
        }
        else
        {

        if (response_buffer.status_code != 200)
        {
            ESP_LOGE(TAG, "HTTP GET request failed with status code: %d", response_buffer.status_code);
            http_rest_client_cleanup_json(&response_buffer);
        }
        else
        {
            char *jsonString = cJSON_Print(response_buffer.json);
            ESP_LOGI(TAG, "Response: %s", jsonString);

            free(jsonString);
            http_rest_client_cleanup_json(&response_buffer);
        }
        }
        ESP_LOGI(TAG, "Looping in 3 sec...");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        }
}

  void WIFIConnection(void)
{
      // create some variables we'll need
    esp_err_t ret = ESP_OK;
    // char *response_body;

    ESP_LOGI(TAG, "Starting app_main...");

    // initialize wifi event group
    wifi_event_group = xEventGroupCreate();

    // Initialize NVS flash for wifi
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    // Check if wer're good to proceed
    ESP_ERROR_CHECK(ret);

    // Create the default event loop for WiFi driver
    ESP_ERROR_CHECK(esp_event_loop_create_default());

  // Initialize network interface for WiFi Station mode
    ESP_ERROR_CHECK(esp_netif_init());
    (void)esp_netif_create_default_wifi_sta();

  // Configure WiFi radio
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // Register WiFi event handlers
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));

  // Set WiFi Station configuration
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false},
        },
    };

  // Set WiFi mode to Station mode
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

  // Start the WiFi driver
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

  // Wait for WiFi connection and ip address before continuing
    ESP_LOGI(TAG, "Waiting for WiFi connection...");
    (void)xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT | WIFI_GOT_IP_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    ESP_LOGI(TAG, "WiFi connected");

    ESP_LOGI(TAG, "Starting Main Loop...");
}