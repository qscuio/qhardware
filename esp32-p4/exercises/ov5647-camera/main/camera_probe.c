#include <fcntl.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_video_device.h"
#include "esp_video_init.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "linux/videodev2.h"

#define CAMERA_I2C_PORT 0
#define CAMERA_I2C_SCL  8
#define CAMERA_I2C_SDA  7
#define CAMERA_I2C_HZ   100000
#define BUFFER_COUNT    2

static const char *TAG = "camera_probe";

typedef struct {
    void *address;
    size_t length;
} mapped_buffer_t;

static uint32_t sampled_checksum(const uint8_t *data, size_t length)
{
    uint32_t hash = 2166136261u;
    size_t step = length > 4096 ? length / 4096 : 1;

    for (size_t i = 0; i < length; i += step) {
        hash ^= data[i];
        hash *= 16777619u;
    }
    return hash;
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting ESP32-P4 MIPI-CSI camera probe");
    ESP_LOGI(TAG, "SCCB/I2C: port=%d SCL=%d SDA=%d", CAMERA_I2C_PORT,
             CAMERA_I2C_SCL, CAMERA_I2C_SDA);

    const esp_video_init_csi_config_t csi_config = {
        .sccb_config = {
            .init_sccb = true,
            .i2c_config = {
                .port = CAMERA_I2C_PORT,
                .scl_pin = CAMERA_I2C_SCL,
                .sda_pin = CAMERA_I2C_SDA,
            },
            .freq = CAMERA_I2C_HZ,
        },
        .reset_pin = -1,
        .pwdn_pin = -1,
    };
    const esp_video_init_config_t video_config = {
        .csi = &csi_config,
    };

    esp_err_t result = esp_video_init(&video_config);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "CAMERA TEST FAIL: sensor initialization returned %s",
                 esp_err_to_name(result));
        return;
    }

    int camera = open(ESP_VIDEO_MIPI_CSI_DEVICE_NAME, O_RDONLY);
    if (camera < 0) {
        ESP_LOGE(TAG, "CAMERA TEST FAIL: could not open %s",
                 ESP_VIDEO_MIPI_CSI_DEVICE_NAME);
        return;
    }

    struct v4l2_capability capability = {0};
    if (ioctl(camera, VIDIOC_QUERYCAP, &capability) != 0) {
        ESP_LOGE(TAG, "CAMERA TEST FAIL: VIDIOC_QUERYCAP failed");
        close(camera);
        return;
    }

    struct v4l2_format format = {
        .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
    };
    if (ioctl(camera, VIDIOC_G_FMT, &format) != 0) {
        ESP_LOGE(TAG, "CAMERA TEST FAIL: VIDIOC_G_FMT failed");
        close(camera);
        return;
    }

    ESP_LOGI(TAG, "Sensor driver=%s card=%s bus=%s", capability.driver,
             capability.card, capability.bus_info);
    ESP_LOGI(TAG, "Frame format: %" PRIu32 "x%" PRIu32 ", fourcc=0x%08" PRIx32,
             format.fmt.pix.width, format.fmt.pix.height,
             format.fmt.pix.pixelformat);

    struct v4l2_requestbuffers request = {
        .count = BUFFER_COUNT,
        .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
        .memory = V4L2_MEMORY_MMAP,
    };
    if (ioctl(camera, VIDIOC_REQBUFS, &request) != 0 || request.count < BUFFER_COUNT) {
        ESP_LOGE(TAG, "CAMERA TEST FAIL: frame buffer allocation failed");
        close(camera);
        return;
    }

    mapped_buffer_t buffers[BUFFER_COUNT] = {0};
    for (uint32_t i = 0; i < BUFFER_COUNT; ++i) {
        struct v4l2_buffer buffer = {
            .index = i,
            .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
            .memory = V4L2_MEMORY_MMAP,
        };
        if (ioctl(camera, VIDIOC_QUERYBUF, &buffer) != 0) {
            ESP_LOGE(TAG, "CAMERA TEST FAIL: buffer query failed at index %" PRIu32, i);
            close(camera);
            return;
        }
        buffers[i].length = buffer.length;
        buffers[i].address = mmap(NULL, buffer.length, PROT_READ | PROT_WRITE,
                                  MAP_SHARED, camera, buffer.m.offset);
        if (buffers[i].address == MAP_FAILED || buffers[i].address == NULL) {
            ESP_LOGE(TAG, "CAMERA TEST FAIL: mmap failed at index %" PRIu32, i);
            close(camera);
            return;
        }
        if (ioctl(camera, VIDIOC_QBUF, &buffer) != 0) {
            ESP_LOGE(TAG, "CAMERA TEST FAIL: queue failed at index %" PRIu32, i);
            close(camera);
            return;
        }
    }

    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(camera, VIDIOC_STREAMON, &type) != 0) {
        ESP_LOGE(TAG, "CAMERA TEST FAIL: stream start failed");
        close(camera);
        return;
    }

    struct v4l2_buffer captured = {
        .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
        .memory = V4L2_MEMORY_MMAP,
    };
    if (ioctl(camera, VIDIOC_DQBUF, &captured) != 0 || captured.index >= BUFFER_COUNT) {
        ESP_LOGE(TAG, "CAMERA TEST FAIL: no frame received");
        ioctl(camera, VIDIOC_STREAMOFF, &type);
        close(camera);
        return;
    }

    size_t bytes = captured.bytesused ? captured.bytesused : buffers[captured.index].length;
    uint32_t checksum = sampled_checksum(buffers[captured.index].address, bytes);
    ESP_LOGI(TAG, "Captured frame: %u bytes, sampled checksum=0x%08" PRIx32,
             (unsigned)bytes, checksum);
    ESP_LOGI(TAG, "CAMERA TEST PASS: sensor detected and a MIPI-CSI frame was received");

    ioctl(camera, VIDIOC_STREAMOFF, &type);
    close(camera);

    while (true) {
        ESP_LOGI(TAG, "Camera probe alive");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
