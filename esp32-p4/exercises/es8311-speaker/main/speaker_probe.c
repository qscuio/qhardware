#include <stdint.h>
#include <stdlib.h>

#include "bsp/esp-bsp.h"
#include "esp_check.h"
#include "esp_codec_dev.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SAMPLE_RATE_HZ 16000
#define CHANNELS 2
#define FRAMES_PER_CHUNK 1024
#define TRACK_BYTES 640000
#define OUTPUT_VOLUME 60

static const char *TAG = "speaker_probe";

static void fill_tone(int16_t *samples, size_t frames)
{
    static uint32_t phase;
    for (size_t frame = 0; frame < frames; ++frame) {
        int16_t value = (phase < SAMPLE_RATE_HZ / 880) ? 5000 : -5000;
        phase = (phase + 1) % (SAMPLE_RATE_HZ / 440);
        samples[frame * CHANNELS] = value;
        samples[frame * CHANNELS + 1] = value;
    }
}

void app_main(void)
{
    esp_codec_dev_handle_t speaker = bsp_audio_codec_speaker_init();
    ESP_ERROR_CHECK_WITHOUT_ABORT(speaker ? ESP_OK : ESP_FAIL);
    if (!speaker) {
        ESP_LOGE(TAG, "SPEAKER TEST FAIL: ES8311 speaker initialization failed");
        return;
    }

    const esp_codec_dev_sample_info_t format = {
        .sample_rate = SAMPLE_RATE_HZ,
        .channel = CHANNELS,
        .bits_per_sample = 16,
    };
    ESP_ERROR_CHECK(esp_codec_dev_open(speaker, &format));
    ESP_ERROR_CHECK(esp_codec_dev_set_out_vol(speaker, OUTPUT_VOLUME));
    ESP_LOGI(TAG, "ES8311 speaker ready: %d Hz, stereo, 16-bit, volume=%d",
             SAMPLE_RATE_HZ, OUTPUT_VOLUME);

    const size_t chunk_bytes = FRAMES_PER_CHUNK * CHANNELS * sizeof(int16_t);
    int16_t *samples = heap_caps_malloc(chunk_bytes, MALLOC_CAP_DMA);
    if (!samples) {
        ESP_LOGE(TAG, "SPEAKER TEST FAIL: DMA buffer allocation failed");
        return;
    }

    while (true) {
        size_t total = 0;
        while (total < TRACK_BYTES) {
            size_t remaining = TRACK_BYTES - total;
            size_t bytes = remaining < chunk_bytes ? remaining : chunk_bytes;
            fill_tone(samples, bytes / (CHANNELS * sizeof(int16_t)));
            ESP_ERROR_CHECK(esp_codec_dev_write(speaker, samples, bytes));
            total += bytes;
        }
        ESP_LOGI(TAG, "SPEAKER TEST PASS: wrote %u bytes", (unsigned)total);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
