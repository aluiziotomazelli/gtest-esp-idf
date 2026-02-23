![Build Status](https://github.com/aluiziotomazelli/gtest-esp-idf/actions/workflows/02_build_esp32.yml/badge.svg)
![Host Tests Status](https://github.com/aluiziotomazelli/gtest-esp-idf/actions/workflows/02_host_tests.yml/badge.svg)

# 02_idf_mocks: using native ESP-IDF types in host tests

This chapter builds on [01_basic_test](../01_basic_test/README.md). The project structure, GTest wrapper, and CMake setup are explained there — this README covers only what changed.

The goal here is to use native ESP-IDF types like `esp_err_t` and macros like `ESP_LOGE` in a component that still compiles and runs on Linux for testing. In this example it's not critical — the logic is simple. But once you start calling lower-level functions like `gpio_set_direction()`, which return `esp_err_t`, being able to propagate and test those error codes all the way up to the application layer is what makes host testing actually useful.

---

## Additional prerequisite: Ruby

IDF's driver mock requires Ruby to generate its mock files at build time.

Check if you have it:

```bash
ruby --version
```

If not:

```bash
sudo apt install ruby-full
```

---

## What changed

### include/i_sum.hpp

A new method was added to the interface:

```cpp
#include "esp_err.h"

virtual esp_err_t add_constrained_err(int a, int b, int &result) = 0;
```

The result is returned through a reference argument — the return value is now reserved for the error code. This is the standard pattern in ESP-IDF drivers, and it's what allows error codes to travel from driver level up to the application.

### CMakeLists.txt (component root)

```cmake
REQUIRES driver
```

Required to bring in `esp_err_t` and the `ESP_LOGx` macros.

### src/sum.cpp

```cpp
#include "esp_log.h"

esp_err_t Sum::add_constrained_err(int a, int b, int &result)
{
    result = -1; // Invalid result on initialization
    if (a < 0 || a > 10 || b < 0 || b > 10) {
        esp_err_t err = ESP_ERR_INVALID_ARG;
        ESP_LOGE(TAG, "Invalid params: a = %d, b = %d, error = %s", a, b, esp_err_to_name(err));
        return err;
    }
    int sum = a + b;
    if (sum > 10) {
        esp_err_t err = ESP_FAIL;
        ESP_LOGE(TAG, "Invalid result: sum = %d, error=%s", sum, esp_err_to_name(err));
        return err;
    }
    result = sum;
    return ESP_OK;
}
```

Same logic as `add_constrained`, but returns `esp_err_t` and emits logs on error via `ESP_LOGE`.

---

## Test project changes

### EXTRA_COMPONENT_DIRS

```cmake
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/driver")
```

Points the build system to the IDF driver mock, which provides `esp_err_t` and the logging macros for the Linux target.

### Test files

Two files cover the new method:

**test_sum.cpp** — extends the previous tests with new cases for `add_constrained_err`, checking the correct `esp_err_t` return codes for each scenario.

**test_sum_param.cpp** — the same tests written as parameterized tests. When the logic is the same and only the inputs change, this keeps the file clean and easier to extend.

For the test strategy and what each test covers, see the [test_sum README](host_test/test_sum/README.md).

---

## Running the tests

```bash
cd 02_idf_mocks/host_test/test_sum
idf.py --preview set-target linux
idf.py build
./build/test_sum.elf
```
Expected result:
```text
[==========] Running 17 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 9 tests from TestSum
...
[----------] 8 tests from SumParamTest/SumParamTest
...
[----------] Global test environment tear-down
[==========] 17 tests from 2 test suites ran. (1 ms total)
[  PASSED  ] 17 tests.
```

---

## Seeing it on real hardware

`test_apps/test_build/` has a working example that calls `add_constrained_err` and prints the results — including the error logs — via serial. To flash and monitor:

```bash
cd 02_idf_mocks/test_apps/test_build
idf.py set-target esp32
idf.py build
idf.py flash monitor
```
