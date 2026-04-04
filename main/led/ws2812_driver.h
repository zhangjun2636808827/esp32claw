#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

esp_err_t ws2812_driver_init(void);
esp_err_t ws2812_driver_set_rgb(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness);
esp_err_t ws2812_driver_off(void);
esp_err_t ws2812_driver_start_breathing(uint8_t red, uint8_t green, uint8_t blue,
                                        uint8_t max_brightness, uint32_t period_ms);
esp_err_t ws2812_driver_stop_breathing(void);
bool ws2812_driver_is_breathing(void);
uint32_t ws2812_driver_get_breath_period_ms(void);
void ws2812_driver_get_current_rgb(uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *brightness);
