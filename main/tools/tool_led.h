#pragma once

#include "esp_err.h"
#include <stddef.h>

esp_err_t tool_led_init(void);
esp_err_t tool_set_led_color_execute(const char *input_json, char *output, size_t output_size);
esp_err_t tool_led_off_execute(const char *input_json, char *output, size_t output_size);
esp_err_t tool_breathing_led_on_execute(const char *input_json, char *output, size_t output_size);
esp_err_t tool_breathing_led_off_execute(const char *input_json, char *output, size_t output_size);
esp_err_t tool_breathing_led_status_execute(const char *input_json, char *output, size_t output_size);
