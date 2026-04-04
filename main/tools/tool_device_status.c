#include "tools/tool_device_status.h"
#include "wifi/wifi_manager.h"
#include "led/ws2812_driver.h"

#include "esp_heap_caps.h"
#include "esp_timer.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static void format_uptime(char *buf, size_t size)
{
    uint64_t uptime_s = (uint64_t)(esp_timer_get_time() / 1000000ULL);
    uint64_t days = uptime_s / 86400ULL;
    uint64_t hours = (uptime_s % 86400ULL) / 3600ULL;
    uint64_t mins = (uptime_s % 3600ULL) / 60ULL;
    uint64_t secs = uptime_s % 60ULL;

    if (days > 0) {
        snprintf(buf, size, "%llud %lluh %llum %llus",
                 (unsigned long long)days,
                 (unsigned long long)hours,
                 (unsigned long long)mins,
                 (unsigned long long)secs);
    } else if (hours > 0) {
        snprintf(buf, size, "%lluh %llum %llus",
                 (unsigned long long)hours,
                 (unsigned long long)mins,
                 (unsigned long long)secs);
    } else if (mins > 0) {
        snprintf(buf, size, "%llum %llus",
                 (unsigned long long)mins,
                 (unsigned long long)secs);
    } else {
        snprintf(buf, size, "%llus", (unsigned long long)secs);
    }
}

esp_err_t tool_device_status_execute(const char *input_json, char *output, size_t output_size)
{
    (void)input_json;

    char uptime[64];
    format_uptime(uptime, sizeof(uptime));

    size_t internal_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    size_t psram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

    uint8_t red = 0, green = 0, blue = 0, brightness = 0;
    ws2812_driver_get_current_rgb(&red, &green, &blue, &brightness);
    bool breathing = ws2812_driver_is_breathing();
    uint32_t breath_period_ms = ws2812_driver_get_breath_period_ms();

    snprintf(output, output_size,
             "Device status:\n"
             "- WiFi: %s\n"
             "- IP: %s\n"
             "- Uptime: %s\n"
             "- Internal free memory: %u bytes\n"
             "- PSRAM free memory: %u bytes\n"
             "- LED mode: %s\n"
             "- LED color: rgb(%u,%u,%u)\n"
             "- LED brightness: %u\n"
             "- Breathing period: %u ms",
             wifi_manager_is_connected() ? "connected" : "disconnected",
             wifi_manager_get_ip(),
             uptime,
             (unsigned)internal_free,
             (unsigned)psram_free,
             breathing ? "breathing" : ((red || green || blue || brightness) ? "static" : "off"),
             (unsigned)red,
             (unsigned)green,
             (unsigned)blue,
             (unsigned)brightness,
             (unsigned)breath_period_ms);

    return ESP_OK;
}
