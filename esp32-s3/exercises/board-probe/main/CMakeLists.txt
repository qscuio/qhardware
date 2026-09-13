idf_component_register(
    SRCS "probe_main.c"
    INCLUDE_DIRS "."
    REQUIRES esp_wifi esp_netif nvs_flash esp_psram bt spi_flash
)
