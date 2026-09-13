#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "esp_chip_info.h"
#include "esp_err.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_psram.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "nvs_flash.h"

static const char *TAG = "QHW_PROBE";
static const char *BLE_NAME = "QHW-ESP32S3-PROBE";
static uint8_t ble_own_addr_type;
static volatile bool ble_advertising;

static uint8_t test_pattern(size_t index)
{
    return (uint8_t)(((index * 33U) ^ (index >> 8U) ^ 0xA5U) & 0xFFU);
}

static bool test_flash(void)
{
    uint32_t flash_size = 0;
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        ESP_LOGE(TAG, "FLASH TEST FAIL: size query failed");
        return false;
    }

    uint8_t *buffer = malloc(4096);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "FLASH TEST FAIL: buffer allocation failed");
        return false;
    }

    uint32_t digest = 2166136261U;
    esp_err_t err = ESP_OK;
    for (uint32_t offset = 0; offset < 64U * 1024U; offset += 4096U) {
        err = esp_flash_read(NULL, buffer, offset, 4096U);
        if (err != ESP_OK) {
            break;
        }
        for (size_t i = 0; i < 4096U; ++i) {
            digest = (digest ^ buffer[i]) * 16777619U;
        }
    }
    free(buffer);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "FLASH TEST FAIL: read error %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(TAG, "FLASH TEST PASS: size=%" PRIu32 "MB read=64KB digest=%08" PRIx32,
             flash_size / (1024U * 1024U), digest);
    return flash_size == 16U * 1024U * 1024U;
}

static bool test_psram(void)
{
    const size_t detected = esp_psram_get_size();
    const size_t heap_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    const size_t heap_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    size_t test_size = 4U * 1024U * 1024U;
    if (test_size > heap_free / 2U) {
        test_size = heap_free / 2U;
    }

    uint8_t *memory = heap_caps_malloc(test_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (memory == NULL || test_size < 1024U * 1024U) {
        free(memory);
        ESP_LOGE(TAG, "PSRAM TEST FAIL: detected=%uKB total=%uKB free=%uKB",
                 (unsigned)(detected / 1024U), (unsigned)(heap_total / 1024U),
                 (unsigned)(heap_free / 1024U));
        return false;
    }

    for (size_t i = 0; i < test_size; ++i) {
        memory[i] = test_pattern(i);
    }
    for (size_t i = 0; i < test_size; ++i) {
        if (memory[i] != test_pattern(i)) {
            ESP_LOGE(TAG, "PSRAM TEST FAIL: mismatch at byte %u", (unsigned)i);
            heap_caps_free(memory);
            return false;
        }
    }
    heap_caps_free(memory);

    ESP_LOGI(TAG, "PSRAM TEST PASS: detected=%uMB heap=%uKB tested=%uKB",
             (unsigned)(detected / (1024U * 1024U)), (unsigned)(heap_total / 1024U),
             (unsigned)(test_size / 1024U));
    return detected == 8U * 1024U * 1024U;
}

static void freertos_worker(void *parameter)
{
    TaskHandle_t caller = (TaskHandle_t)parameter;
    vTaskDelay(pdMS_TO_TICKS(100));
    xTaskNotifyGive(caller);
    vTaskDelete(NULL);
}

static bool test_freertos(void)
{
    TickType_t start = xTaskGetTickCount();
    BaseType_t created = xTaskCreate(freertos_worker, "probe_worker", 2048,
                                     xTaskGetCurrentTaskHandle(), 5, NULL);
    if (created != pdPASS || ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(2000)) == 0) {
        ESP_LOGE(TAG, "FREERTOS TEST FAIL");
        return false;
    }
    TickType_t elapsed = xTaskGetTickCount() - start;
    ESP_LOGI(TAG, "FREERTOS TEST PASS: cores=%u worker_delay_ticks=%" PRIu32,
             (unsigned)CONFIG_FREERTOS_NUMBER_OF_CORES, (uint32_t)elapsed);
    return true;
}

static bool test_wifi_scan(void)
{
    esp_err_t err = esp_netif_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "WIFI SCAN FAIL: netif init %s", esp_err_to_name(err));
        return false;
    }
    err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "WIFI SCAN FAIL: event loop %s", esp_err_to_name(err));
        return false;
    }

    esp_netif_t *station = esp_netif_create_default_wifi_sta();
    wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
    if (station == NULL || esp_wifi_init(&init) != ESP_OK ||
        esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK || esp_wifi_start() != ESP_OK) {
        ESP_LOGE(TAG, "WIFI SCAN FAIL: radio initialization");
        return false;
    }

    wifi_scan_config_t scan = {
        .show_hidden = false,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
    };
    err = esp_wifi_scan_start(&scan, true);
    uint16_t count = 0;
    if (err == ESP_OK) {
        err = esp_wifi_scan_get_ap_num(&count);
    }

    esp_wifi_stop();
    esp_wifi_deinit();
    esp_netif_destroy_default_wifi(station);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "WIFI SCAN FAIL: %s", esp_err_to_name(err));
        return false;
    }
    ESP_LOGI(TAG, "WIFI SCAN PASS: visible_access_points=%u (names intentionally hidden)",
             (unsigned)count);
    return true;
}

static int ble_gap_event(struct ble_gap_event *event, void *arg);

static void start_ble_advertising(void)
{
    struct ble_hs_adv_fields fields = {0};
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.name = (uint8_t *)BLE_NAME;
    fields.name_len = strlen(BLE_NAME);
    fields.name_is_complete = 1;

    int rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "BLE TEST FAIL: advertisement data rc=%d", rc);
        return;
    }

    struct ble_gap_adv_params params = {0};
    params.conn_mode = BLE_GAP_CONN_MODE_UND;
    params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(ble_own_addr_type, NULL, BLE_HS_FOREVER, &params,
                           ble_gap_event, NULL);
    if (rc == 0) {
        ble_advertising = true;
        ESP_LOGI(TAG, "BLE ADVERTISE PASS: name=%s", BLE_NAME);
        ESP_LOGI(TAG, "ALL ON-BOARD PROBE TESTS STARTED");
    } else {
        ESP_LOGE(TAG, "BLE TEST FAIL: start rc=%d", rc);
    }
}

static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    (void)arg;
    if (event->type == BLE_GAP_EVENT_DISCONNECT ||
        event->type == BLE_GAP_EVENT_ADV_COMPLETE ||
        (event->type == BLE_GAP_EVENT_CONNECT && event->connect.status != 0)) {
        start_ble_advertising();
    }
    return 0;
}

static void ble_on_reset(int reason)
{
    ESP_LOGE(TAG, "BLE host reset: reason=%d", reason);
}

static void ble_on_sync(void)
{
    int rc = ble_hs_util_ensure_addr(0);
    if (rc == 0) {
        rc = ble_hs_id_infer_auto(0, &ble_own_addr_type);
    }
    if (rc != 0) {
        ESP_LOGE(TAG, "BLE TEST FAIL: identity rc=%d", rc);
        return;
    }
    start_ble_advertising();
}

static void ble_host_task(void *parameter)
{
    (void)parameter;
    nimble_port_run();
    nimble_port_freertos_deinit();
}

static void start_ble_test(void)
{
    esp_err_t err = nimble_port_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "BLE TEST FAIL: init %s", esp_err_to_name(err));
        return;
    }
    ble_hs_cfg.reset_cb = ble_on_reset;
    ble_hs_cfg.sync_cb = ble_on_sync;
    nimble_port_freertos_init(ble_host_task);
}

void app_main(void)
{
    esp_chip_info_t chip = {0};
    esp_chip_info(&chip);
    ESP_LOGI(TAG, "PROBE START: model=%d revision=%u cores=%u",
             chip.model, (unsigned)chip.revision, (unsigned)chip.cores);

    esp_err_t nvs = nvs_flash_init();
    if (nvs == ESP_ERR_NVS_NO_FREE_PAGES || nvs == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs);

    bool flash_ok = test_flash();
    bool psram_ok = test_psram();
    bool freertos_ok = test_freertos();
    bool wifi_ok = test_wifi_scan();
    ESP_LOGI(TAG, "CORE TEST SUMMARY: flash=%s psram=%s freertos=%s wifi=%s",
             flash_ok ? "PASS" : "FAIL", psram_ok ? "PASS" : "FAIL",
             freertos_ok ? "PASS" : "FAIL", wifi_ok ? "PASS" : "FAIL");

    start_ble_test();

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP_LOGI(TAG,
                 "PROBE HEARTBEAT: flash=%s psram=%s freertos=%s wifi=%s ble=%s",
                 flash_ok ? "PASS" : "FAIL", psram_ok ? "PASS" : "FAIL",
                 freertos_ok ? "PASS" : "FAIL", wifi_ok ? "PASS" : "FAIL",
                 ble_advertising ? "ACTIVE" : "PENDING");
    }
}
