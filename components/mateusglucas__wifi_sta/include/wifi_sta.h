#pragma once

#include "esp_err.h"
#include "esp_wifi.h"

#define USE_USER_WIFI 0
#if USE_USER_WIFI
#define WIFI_SSID       "Mi10Pro"
#define WIFI_PASSWORD   "12345687"
#else
#define WIFI_SSID       "耶耶"
#define WIFI_PASSWORD   "12345687!"
#endif

void wifi_connect(void);

/**
 * @brief Initialize Wi-Fi station
 * @return ESP_OK on success, otherwise ESP_ERR code
 */
esp_err_t wifi_sta_init(void);

/**
 * @brief Set Wi-Fi station cofigurations
 * @param sta_config Wi-Fi station configurations
 * @return ESP_OK on success, otherwise ESP_ERR code
 */
esp_err_t wifi_sta_set_config(wifi_sta_config_t *sta_config);

/**
 * @brief Connect to Wi-Fi
 * @return ESP_OK on success, otherwise ESP_ERR code
 */
esp_err_t wifi_sta_connect(void);