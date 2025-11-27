#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/touch_pad.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_err.h"

/* TinyUSB headers - ESP-IDF provides TinyUSB as a component. The example below
   uses the common TinyUSB device HID helpers such as `tud_hid_keyboard_report`
   and `tud_remote_wakeup`. If your ESP-IDF version provides slightly
   different names, adapt accordingly. */
#include "tusb.h"

static const char *TAG = "capacity_wakeup";

// Touch pad selection: can be overridden via `menuconfig` (see main/Kconfig)
#define TOUCH_PAD_SEL (TOUCH_PAD_NUM##CONFIG_TOUCH_PAD_NUMBER)

// Configuration
#define BASELINE_SAMPLES 20
#define POLL_MS 100

// Debounce / Hysteresis configuration
#define DEBOUNCE_COUNT 3
#define ON_FACTOR 0.70f    // threshold to detect touch (val < on_threshold)
#define OFF_FACTOR 0.85f   // threshold to detect release (val >= off_threshold)

// Optional LED feedback: controlled via menuconfig `CONFIG_LED_GPIO`.
// Set to -1 to disable. Default set in `menuconfig` (default: 48).
#ifndef CONFIG_LED_GPIO
#define CONFIG_LED_GPIO -1
#endif
#define LED_GPIO CONFIG_LED_GPIO

// HID key to send to wake the host (HID keycode for space)
#define HID_KEY_SPACE 0x2C

static uint16_t baseline = 0;
static float threshold = 0;
static float on_threshold = 0;
static float off_threshold = 0;

static void init_touchpad(void)
{
    esp_err_t ret = touch_pad_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "touch_pad_init failed: %d", ret);
        return;
    }

    // Configure selected pad with default attenuation
    touch_pad_config(TOUCH_PAD_SEL, 0);

    // Optional: enable filter to stabilize readings
    touch_pad_filter_start(10);
    ESP_LOGI(TAG, "Touch pad initialized (pad %d)", TOUCH_PAD_SEL);
}

static uint16_t read_touch_filtered(void)
{
    uint16_t val = 0;
    touch_pad_read_filtered(TOUCH_PAD_SEL, &val);
    return val;
}

static void init_tinyusb_hid(void)
{
    // Initialize TinyUSB stack (ESP-IDF wrapper)
    // Call the stack init function for device mode. API names differ across
    // versions; the following works with standard TinyUSB usage in ESP-IDF.
    tusb_init();
    ESP_LOGI(TAG, "TinyUSB initialized");
}

static void send_hid_space_press(void)
{
    // If host is suspended, request remote wakeup first
    if (tud_suspended()) {
        ESP_LOGI(TAG, "Host suspended, requesting remote wakeup");
        // Will return true if host accepted remote wakeup request
        tud_remote_wakeup();
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Send key press (space) and release
    uint8_t keycode[6] = {0};
    keycode[0] = HID_KEY_SPACE;
    tud_hid_keyboard_report(0, 0, keycode);
    vTaskDelay(pdMS_TO_TICKS(10));
    memset(keycode, 0, sizeof(keycode));
    tud_hid_keyboard_report(0, 0, keycode);

    ESP_LOGI(TAG, "Sent HID space press");
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting CapacityWakeup");

    init_touchpad();
    init_tinyusb_hid();

    // Build baseline reading
    uint32_t sum = 0;
    for (int i = 0; i < BASELINE_SAMPLES; ++i) {
        uint16_t v = read_touch_filtered();
        sum += v;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    baseline = sum / BASELINE_SAMPLES;
    // Initial thresholds using factors
    on_threshold = baseline * ON_FACTOR;
    off_threshold = baseline * OFF_FACTOR;
    ESP_LOGI(TAG, "Baseline: %d  OnThreshold: %.1f  OffThreshold: %.1f", baseline, on_threshold, off_threshold);

    // Optional LED init
    if (LED_GPIO >= 0) {
        gpio_reset_pin(LED_GPIO);
        gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_level(LED_GPIO, 0);
    }

    int touch_confirm = 0;
    bool touched_state = false;

#if CONFIG_ENABLE_RAW_LOG
    // Raw logging mode: print raw values each poll for autotuning tools.
    ESP_LOGI(TAG, "RAW LOG MODE ENABLED - printing raw values each %d ms", POLL_MS);
    while (1) {
        uint16_t val = read_touch_filtered();
        // Print a simple RAW: prefix so external tools can parse easily
        printf("RAW: %u\n", val);
        vTaskDelay(pdMS_TO_TICKS(POLL_MS));
    }
#else
    while (1) {
        uint16_t val = read_touch_filtered();

        if (!touched_state) {
            // Waiting for touch: compare against on_threshold
            if (val < on_threshold) {
                touch_confirm++;
                if (touch_confirm >= DEBOUNCE_COUNT) {
                    touched_state = true;
                    touch_confirm = 0;
                    ESP_LOGI(TAG, "Touch detected (val=%d)", val);
                    send_hid_space_press();
                    if (LED_GPIO >= 0) gpio_set_level(LED_GPIO, 1);
                }
            } else {
                touch_confirm = 0;
            }
        } else {
            // Currently touched: wait for release using off_threshold (hysteresis)
            if (val >= off_threshold) {
                touch_confirm++;
                if (touch_confirm >= DEBOUNCE_COUNT) {
                    touched_state = false;
                    touch_confirm = 0;
                    ESP_LOGI(TAG, "Touch released (val=%d)", val);
                    if (LED_GPIO >= 0) gpio_set_level(LED_GPIO, 0);
                }
            } else {
                touch_confirm = 0;
            }
        }

        // Update baseline slowly when not touched to adapt to drift
        if (!touched_state) {
            baseline = (baseline * 99 + val) / 100;
            on_threshold = baseline * ON_FACTOR;
            off_threshold = baseline * OFF_FACTOR;
        }

        vTaskDelay(pdMS_TO_TICKS(POLL_MS));
    }
#endif
}
