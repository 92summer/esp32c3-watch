FILE(GLOB_RECURSE app_sources "./src/*.c")

idf_component_register(
                    SRCS ${app_sources}
                    INCLUDE_DIRS "./include" "./examples/http_get_and_parse_json/main/include"
                    REQUIRES esp_http_client esp-tls json esp_netif esp_wifi nvs_flash
                    )
