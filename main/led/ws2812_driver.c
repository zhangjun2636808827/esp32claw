#include "led/ws2812_driver.h"
#include "mimi_config.h"

#include "led_strip.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include <stdbool.h>
#include <stdint.h>

static const char *TAG = "ws2812";

static led_strip_handle_t s_strip;
static bool s_ready = false;
static TaskHandle_t s_breath_task = NULL;
static volatile bool s_breathing_enabled = false;
static uint8_t s_breath_red = 0;
static uint8_t s_breath_green = 0;
static uint8_t s_breath_blue = 255;
static uint8_t s_breath_max_brightness = 128;
static uint32_t s_breath_period_ms = MIMI_WS2812_BREATH_PERIOD_MS;
static uint8_t s_current_red = 0;
static uint8_t s_current_green = 0;
static uint8_t s_current_blue = 0;
static uint8_t s_current_brightness = 0;
static SemaphoreHandle_t s_led_mutex = NULL;

static esp_err_t ws2812_apply_rgb(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness);

static bool ws2812_lock(TickType_t timeout_ticks)
{
    if (!s_led_mutex) {
        return false;
    }
    return xSemaphoreTake(s_led_mutex, timeout_ticks) == pdTRUE;
}

static void ws2812_unlock(void)
{
    if (s_led_mutex) {
        xSemaphoreGive(s_led_mutex);
    }
}

static void ws2812_breath_task(void *arg)
{
    (void)arg;

    while (1) {
        if (!s_breathing_enabled) {
            ws2812_apply_rgb(0, 0, 0, 0);
            s_breath_task = NULL;
            vTaskDelete(NULL);
            return;
        }

        TickType_t now_ticks = xTaskGetTickCount();
        uint32_t now_ms = (uint32_t)(now_ticks * portTICK_PERIOD_MS);
        uint32_t period_ms = s_breath_period_ms > 0 ? s_breath_period_ms : MIMI_WS2812_BREATH_PERIOD_MS;
        uint32_t phase_ms = now_ms % period_ms;
        uint32_t numerator;

        if (phase_ms < (period_ms / 2U)) {
            numerator = phase_ms * 2U;
        } else {
            numerator = (period_ms - phase_ms) * 2U;
        }

        uint32_t brightness = ((uint32_t)s_breath_max_brightness * numerator) / period_ms;
        ws2812_apply_rgb(s_breath_red, s_breath_green, s_breath_blue, (uint8_t)brightness);
        vTaskDelay(pdMS_TO_TICKS(MIMI_WS2812_BREATH_FRAME_MS));
    }
}

static esp_err_t ws2812_apply_rgb(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
{
    if (!s_strip) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!ws2812_lock(pdMS_TO_TICKS(100))) {
        ESP_LOGW(TAG, "LED mutex timeout");
        return ESP_ERR_TIMEOUT;
    }

    uint16_t scaled_red = ((uint16_t)red * (uint16_t)brightness) / 255;
    uint16_t scaled_green = ((uint16_t)green * (uint16_t)brightness) / 255;
    uint16_t scaled_blue = ((uint16_t)blue * (uint16_t)brightness) / 255;

    esp_err_t err = led_strip_set_pixel(s_strip, 0, scaled_red, scaled_green, scaled_blue);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set pixel: %s", esp_err_to_name(err));
        ws2812_unlock();
        return err;
    }

    err = led_strip_refresh(s_strip);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to refresh strip: %s", esp_err_to_name(err));
        ws2812_unlock();
        return err;
    }

    s_current_red = red;
    s_current_green = green;
    s_current_blue = blue;
    s_current_brightness = brightness;
    ws2812_unlock();

    return ESP_OK;
}

esp_err_t ws2812_driver_init(void)
{
    if (s_ready) {
        return ESP_OK;
    }

    if (!s_led_mutex) {
        s_led_mutex = xSemaphoreCreateMutex();
        if (!s_led_mutex) {
            ESP_LOGE(TAG, "Failed to create LED mutex");
            return ESP_ERR_NO_MEM;
        }
    }

    led_strip_config_t strip_config = {
        .strip_gpio_num = MIMI_WS2812_GPIO,
        .max_leds = MIMI_WS2812_LED_COUNT,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .flags = {
            .invert_out = false,
        },
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = MIMI_WS2812_RMT_RES_HZ,
        .mem_block_symbols = 64,
        .flags = {
            .with_dma = false,
        },
    };

    esp_err_t err = led_strip_new_rmt_device(&strip_config, &rmt_config, &s_strip);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create LED strip device: %s", esp_err_to_name(err));
        return err;
    }

    s_ready = true;
    ESP_LOGI(TAG, "WS2812 initialized on GPIO%d (%d LED)", MIMI_WS2812_GPIO, MIMI_WS2812_LED_COUNT);

    err = ws2812_apply_rgb(MIMI_WS2812_BOOT_RED,
                           MIMI_WS2812_BOOT_GREEN,
                           MIMI_WS2812_BOOT_BLUE,
                           MIMI_WS2812_BOOT_BRIGHTNESS);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "WS2812 boot self-test color: rgb(%d,%d,%d) brightness=%d",
                 MIMI_WS2812_BOOT_RED,
                 MIMI_WS2812_BOOT_GREEN,
                 MIMI_WS2812_BOOT_BLUE,
                 MIMI_WS2812_BOOT_BRIGHTNESS);
    }
    return err;
}

esp_err_t ws2812_driver_set_rgb(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
{
    if (!s_ready) {
        esp_err_t err = ws2812_driver_init();
        if (err != ESP_OK) {
            return err;
        }
    }

    s_breathing_enabled = false;

    esp_err_t err = ws2812_apply_rgb(red, green, blue, brightness);
    if (err != ESP_OK) {
        return err;
    }

    ESP_LOGI(TAG, "LED set to rgb(%u,%u,%u) brightness=%u",
             (unsigned)red, (unsigned)green, (unsigned)blue, (unsigned)brightness);
    return ESP_OK;
}

esp_err_t ws2812_driver_off(void)
{
    if (!s_ready) {
        esp_err_t err = ws2812_driver_init();
        if (err != ESP_OK) {
            return err;
        }
    }

    s_breathing_enabled = false;

    if (!ws2812_lock(pdMS_TO_TICKS(100))) {
        ESP_LOGW(TAG, "LED mutex timeout");
        return ESP_ERR_TIMEOUT;
    }
    esp_err_t err = led_strip_clear(s_strip);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to clear strip: %s", esp_err_to_name(err));
        ws2812_unlock();
        return err;
    }

    s_current_red = 0;
    s_current_green = 0;
    s_current_blue = 0;
    s_current_brightness = 0;
    ws2812_unlock();

    ESP_LOGI(TAG, "LED off");
    return ESP_OK;
}

esp_err_t ws2812_driver_start_breathing(uint8_t red, uint8_t green, uint8_t blue,
                                        uint8_t max_brightness, uint32_t period_ms)
{
    if (!s_ready) {
        esp_err_t err = ws2812_driver_init();
        if (err != ESP_OK) {
            return err;
        }
    }

    s_breath_red = red;
    s_breath_green = green;
    s_breath_blue = blue;
    s_breath_max_brightness = max_brightness > 0 ? max_brightness : 1;
    s_breath_period_ms = period_ms > 0 ? period_ms : MIMI_WS2812_BREATH_PERIOD_MS;
    s_breathing_enabled = true;

    if (s_breath_task == NULL) {
        BaseType_t ok = xTaskCreatePinnedToCore(
            ws2812_breath_task,
            "ws2812_breath",
            3072,
            NULL,
            4,
            &s_breath_task,
            1);
        if (ok != pdPASS) {
            s_breathing_enabled = false;
            s_breath_task = NULL;
            ESP_LOGE(TAG, "Failed to create breathing task");
            return ESP_FAIL;
        }
    }

    ESP_LOGI(TAG, "Breathing LED enabled rgb(%u,%u,%u) max_brightness=%u period=%ums",
             (unsigned)red, (unsigned)green, (unsigned)blue,
             (unsigned)s_breath_max_brightness, (unsigned)s_breath_period_ms);
    return ESP_OK;
}

esp_err_t ws2812_driver_stop_breathing(void)
{
    s_breathing_enabled = false;
    return ws2812_driver_off();
}

bool ws2812_driver_is_breathing(void)
{
    return s_breathing_enabled;
}

uint32_t ws2812_driver_get_breath_period_ms(void)
{
    return s_breath_period_ms;
}

void ws2812_driver_get_current_rgb(uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *brightness)
{
    if (red) {
        *red = s_current_red;
    }
    if (green) {
        *green = s_current_green;
    }
    if (blue) {
        *blue = s_current_blue;
    }
    if (brightness) {
        *brightness = s_current_brightness;
    }
}
