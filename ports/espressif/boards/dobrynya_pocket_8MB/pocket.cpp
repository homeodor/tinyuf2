#include <cstdint>
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "esp_err.h"
#include "rom/ets_sys.h"

static constexpr gpio_num_t k_switchOnPin = GPIO_NUM_10;
static constexpr gpio_num_t k_powerupPin = GPIO_NUM_46;

extern "C" void board_rgb_write(uint8_t const rgb[]);

constexpr uint8_t k_black[] = { 0, 0, 0 };

extern "C" void board_init_extension()
{
    gpio_reset_pin(k_powerupPin);
    gpio_set_direction(k_powerupPin, GPIO_MODE_OUTPUT);
    gpio_set_level(k_powerupPin, 0);

    rtc_gpio_init(k_switchOnPin);
    rtc_gpio_set_direction(k_switchOnPin, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pulldown_dis(k_switchOnPin);
    rtc_gpio_pullup_en(k_switchOnPin);
    rtc_gpio_hold_en(k_switchOnPin);
    ets_delay_us(200);

    // If switch is HIGH, enter deep sleep immediately.
    if (rtc_gpio_get_level(k_switchOnPin) == 1)
    {
        ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(1ULL << k_switchOnPin,
                                                     ESP_EXT1_WAKEUP_ANY_LOW));

        board_rgb_write(k_black);
        ets_delay_us(200);
        esp_deep_sleep_start();

        while (true)
        {
        }
    }

    // Otherwise, power up: drive the powerup pin HIGH.

    gpio_set_level(k_powerupPin, 1);
}
