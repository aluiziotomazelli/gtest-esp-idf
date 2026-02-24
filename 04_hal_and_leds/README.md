![Build Status](https://github.com/aluiziotomazelli/gtest-esp-idf/actions/workflows/04_build_esp32.yml/badge.svg)
![Host Tests Status](https://github.com/aluiziotomazelli/gtest-esp-idf/actions/workflows/04_host_tests.yml/badge.svg)

# 04_hal_and_leds: hardware abstraction and LED feedback

This chapter builds on [03_class_mock](../03_class_mock/README.md). This README covers only what changed.

Two new classes in this chapter: `GpioHal`, the concrete HAL that talks to the ESP-IDF GPIO driver, and `LedSargent`, which controls the LEDs. `SumBoss` is updated to use `LedSargent` and now makes a real decision based on the result.

---

## What changed

### IGpioHal and GpioHal

`IGpioHal` is the interface that abstracts the GPIO driver:

```cpp
virtual esp_err_t pin_set_direction(gpio_num_t gpio_num, gpio_mode_t mode) = 0;
virtual esp_err_t pin_set_level(gpio_num_t gpio_num, uint32_t level) = 0;
```

`GpioHal` is the concrete implementation — it just calls the real ESP-IDF functions:

```cpp
esp_err_t pin_set_direction(gpio_num_t gpio_num, gpio_mode_t mode) override {
    return gpio_set_direction(gpio_num, mode);
}
esp_err_t pin_set_level(gpio_num_t gpio_num, uint32_t level) override {
    return gpio_set_level(gpio_num, level);
}
```

`GpioHal` is not tested — it's a thin wrapper around ESP-IDF, which is already tested by Espressif. Without the interface, there's no way to test `LedSargent` without real hardware.

### ILedSargent and LedSargent

`LedSargent` receives `IGpioHal&` and the two GPIO pin numbers via constructor injection. The constructor immediately configures both pins as outputs:

```cpp
LedSargent::LedSargent(IGpioHal &gpio_hal, gpio_num_t green, gpio_num_t red)
    : gpio_hal_(gpio_hal), green_(green), red_(red)
{
    gpio_hal_.pin_set_direction(green, GPIO_MODE_OUTPUT);
    gpio_hal_.pin_set_direction(red, GPIO_MODE_OUTPUT);
}
```

Three methods: `green()`, `red()`, and `off()`. The `off()` method returns early if the first pin fails — the second pin is not touched in that case.

`LedSargent` receives `IGpioHal&`, not `GpioHal&` directly. Without the interface, `MockGpioHal` wouldn't fit and the class couldn't be tested in isolation.

### SumBoss

`SumBoss` now receives `ILedSargent&` as a second dependency and acts on the result:

```cpp
SumBoss::SumBoss(ISum &sum, ILedSargent &led_sargent)
    : sum_(sum), led_sargent_(led_sargent) {}

esp_err_t SumBoss::compute(int a, int b, int &result)
{
        esp_err_t ret = sum_.add_constrained_err(a, b, result);
    if (ret == ESP_OK) {            // no error
        ret = led_sargent_.green(); // check if green led works
        if (ret != ESP_OK) {        // green led does not work
            return ret;             // return the error
        }
    }
    else {                  // if the sum failed
        led_sargent_.red(); // the sum error is already in ret
    }
    return ret;
}
```

One thing worth explaining: if `green()` fails, the LED error propagates — the caller needs to know the full operation didn't complete. If `red()` fails, the error is ignored — the sum already failed and that's what the caller gets back.

---

## Seeing it on real hardware

`test_apps/test_build/` has a working example with all classes wired together using real dependencies. Valid and invalid inputs are tested, the results printed via serial, and the LEDs light up on the board:

```bash
cd 04_hal_and_leds/test_apps/test_build
idf.py set-target esp32
idf.py build
idf.py flash monitor
```

---

## Running the tests

```bash
cd 04_hal_and_leds/host_test/test_sum
idf.py --preview set-target linux
idf.py build
./build/test_sum.elf
```

For the full test strategy, see the [test_sum README](host_test/test_sum/README.md).
