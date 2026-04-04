#include "tools/tool_led.h"
#include "led/ws2812_driver.h"
#include "mimi_config.h"

#include "esp_log.h"
#include "cJSON.h"

#include <stdint.h>
#include <stdio.h>

static const char *TAG = "tool_led";

static int json_int_or_default(cJSON *root, const char *name, int default_value)
{
    cJSON *item = cJSON_GetObjectItem(root, name);
    return cJSON_IsNumber(item) ? (int)item->valuedouble : default_value;
}

static uint32_t parse_breath_period_ms(cJSON *root)
{
    cJSON *period_ms = cJSON_GetObjectItem(root, "period_ms");
    if (cJSON_IsNumber(period_ms) && period_ms->valuedouble > 0) {
        return (uint32_t)period_ms->valuedouble;
    }

    cJSON *period_s = cJSON_GetObjectItem(root, "period_s");
    if (cJSON_IsNumber(period_s) && period_s->valuedouble > 0) {
        return (uint32_t)(period_s->valuedouble * 1000.0);
    }

    return MIMI_WS2812_BREATH_PERIOD_MS;
}

esp_err_t tool_led_init(void)
{
    esp_err_t err = ws2812_driver_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "LED tool init failed: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "LED tool initialized on GPIO%d", MIMI_WS2812_GPIO);
    return ESP_OK;
}

esp_err_t tool_set_led_color_execute(const char *input_json, char *output, size_t output_size)
{
    cJSON *root = cJSON_Parse(input_json);
    if (!root) {
        snprintf(output, output_size, "Error: invalid JSON input");
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *r_obj = cJSON_GetObjectItem(root, "r");
    cJSON *g_obj = cJSON_GetObjectItem(root, "g");
    cJSON *b_obj = cJSON_GetObjectItem(root, "b");

    if (!cJSON_IsNumber(r_obj) || !cJSON_IsNumber(g_obj) || !cJSON_IsNumber(b_obj)) {
        snprintf(output, output_size, "Error: 'r', 'g', and 'b' are required (0-255)");
        cJSON_Delete(root);
        return ESP_ERR_INVALID_ARG;
    }

    int red = (int)r_obj->valuedouble;
    int green = (int)g_obj->valuedouble;
    int blue = (int)b_obj->valuedouble;
    int brightness = json_int_or_default(root, "brightness", 255);

    if (red < 0 || red > 255 || green < 0 || green > 255 ||
        blue < 0 || blue > 255 || brightness < 0 || brightness > 255) {
        snprintf(output, output_size, "Error: color and brightness values must be 0-255");
        cJSON_Delete(root);
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ws2812_driver_set_rgb((uint8_t)red, (uint8_t)green, (uint8_t)blue, (uint8_t)brightness);
    if (err != ESP_OK) {
        snprintf(output, output_size, "Error: failed to set LED color");
        cJSON_Delete(root);
        return err;
    }

    snprintf(output, output_size,
             "WS2812B on GPIO%d set to rgb(%d,%d,%d) brightness=%d",
             MIMI_WS2812_GPIO, red, green, blue, brightness);
    ESP_LOGI(TAG, "set_led_color -> rgb(%d,%d,%d) brightness=%d", red, green, blue, brightness);

    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t tool_led_off_execute(const char *input_json, char *output, size_t output_size)
{
    (void)input_json;

    esp_err_t err = ws2812_driver_off();
    if (err != ESP_OK) {
        snprintf(output, output_size, "Error: failed to turn LED off");
        return err;
    }

    snprintf(output, output_size, "WS2812B on GPIO%d turned off", MIMI_WS2812_GPIO);
    ESP_LOGI(TAG, "led_off");
    return ESP_OK;
}

esp_err_t tool_breathing_led_on_execute(const char *input_json, char *output, size_t output_size)
{
    cJSON *root = cJSON_Parse(input_json);
    if (!root) {
        snprintf(output, output_size, "Error: invalid JSON input");
        return ESP_ERR_INVALID_ARG;
    }

    int red = json_int_or_default(root, "r", 0);
    int green = json_int_or_default(root, "g", 0);
    int blue = json_int_or_default(root, "b", 255);
    int brightness = json_int_or_default(root, "brightness", 128);
    uint32_t period_ms = parse_breath_period_ms(root);

    if (red < 0 || red > 255 || green < 0 || green > 255 ||
        blue < 0 || blue > 255 || brightness < 1 || brightness > 255 ||
        period_ms < 100 || period_ms > 600000) {
        snprintf(output, output_size, "Error: r/g/b must be 0-255, brightness 1-255, period 100-600000 ms");
        cJSON_Delete(root);
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ws2812_driver_start_breathing((uint8_t)red, (uint8_t)green, (uint8_t)blue,
                                                  (uint8_t)brightness, period_ms);
    if (err != ESP_OK) {
        snprintf(output, output_size, "Error: failed to enable breathing LED");
        cJSON_Delete(root);
        return err;
    }

    snprintf(output, output_size,
             "Breathing LED enabled on GPIO%d with rgb(%d,%d,%d) max_brightness=%d period_ms=%u",
             MIMI_WS2812_GPIO, red, green, blue, brightness, (unsigned)period_ms);
    ESP_LOGI(TAG, "breathing_led_on -> rgb(%d,%d,%d) max_brightness=%d period_ms=%u",
             red, green, blue, brightness, (unsigned)period_ms);

    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t tool_breathing_led_off_execute(const char *input_json, char *output, size_t output_size)
{
    (void)input_json;

    esp_err_t err = ws2812_driver_stop_breathing();
    if (err != ESP_OK) {
        snprintf(output, output_size, "Error: failed to disable breathing LED");
        return err;
    }

    snprintf(output, output_size, "Breathing LED disabled on GPIO%d", MIMI_WS2812_GPIO);
    ESP_LOGI(TAG, "breathing_led_off");
    return ESP_OK;
}

esp_err_t tool_breathing_led_status_execute(const char *input_json, char *output, size_t output_size)
{
    (void)input_json;
    snprintf(output, output_size, "Breathing LED is %s (period_ms=%u)",
             ws2812_driver_is_breathing() ? "ON" : "OFF",
             (unsigned)ws2812_driver_get_breath_period_ms());
    return ESP_OK;
}
