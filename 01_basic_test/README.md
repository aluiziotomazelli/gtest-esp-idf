![Build Status](https://github.com/aluiziotomazelli/gtest-esp-idf/actions/workflows/01_build_esp32.yml/badge.svg)
![Host Tests Status](https://github.com/aluiziotomazelli/gtest-esp-idf/actions/workflows/01_host_tests.yml/badge.svg)

# 01_basic_test: Project Structure and Component Architecture

This folder represents a standalone ESP-IDF component named **01_basic_test**. In the ESP-IDF build system, a component is defined by its directory name and its `CMakeLists.txt` file. This specific component is designed to demonstrate both target-based execution and host-based unit testing.

## Directory Tree

The project is organized as follows:

```text
├── 01_basic_test
│   ├── CMakeLists.txt             # Component build rules
│   ├── host_test                  # Host-based (Linux) unit tests
│   │   ├── gtest                  # GoogleTest framework configuration
│   │   └── test_sum               # Specific test suite for the Sum class
│   ├── include                    # Public headers
│   │   ├── i_sum.hpp              # Interface (Abstract base class)
│   │   └── sum.hpp                # Real implementation header
│   ├── src                        # Source files
│   │   └── sum.cpp                # Logic implementation
│   └── test_apps                  # Verification projects for target hardware
│       └── test_build             # Simple project to verify ESP32 compilation
```

---

## Component Breakdown

### 01_basic_test

This folder, **01_basic_test**, is the root of this tutorial and is designed as a standalone ESP-IDF component. In the ESP-IDF build system, the directory name defines the component name; therefore, the component we are developing and testing is named `01_basic_test`.


### 1. include/
Contains the header files for the component.
* **i_sum.hpp**: Defines the interface using pure virtual functions. While not strictly necessary for simple logic tests, using interfaces is a best practice. It allows us to derive **Mocks** in future tests, decoupling the business logic from its dependencies.
* **sum.hpp**: The header for the concrete `Sum` class.

### 2. src/
Contains the implementation of the logic defined in the headers. This is the code that will eventually run on your production hardware.

### 3. test_apps/test_build/
A minimal ESP-IDF project used to verify that the component compiles and runs correctly on the actual hardware (e.g., ESP32).
* **Purpose**: Validates the integration, memory footprint, and target compatibility.
* **Usage**:
    ```bash
    idf.py set-target esp32
    idf.py build
    idf.py flash monitor
    ```

### 4. host_test/
Dedicated folder for unit tests that run on a Linux environment, providing near-instant feedback without the need for hardware.
* **test_sum/**: Contains the test suite for the `Sum` class. As components grow, each class or logic block should have its own dedicated folder within `host_test` for better organization.
* **gtest/**: This directory contains the `CMakeLists.txt` responsible for configuring and fetching the **GoogleTest** framework, making it available for the test suites.

## Test Project Configuration (test_sum)

The directory `01_basic_test/host_test/test_sum` is a **standalone project**. It follows the standard ESP-IDF project structure but is specifically tailored for host execution.

### 1. Project Level: test_sum/CMakeLists.txt

The top-level CMakeLists in the test folder defines how the project finds its dependencies.

#### Key Logic:
* **EXTRA_COMPONENT_DIRS**: Since the component we are testing (`01_basic_test`) and our GTest wrapper are outside this project's folder, we use `list(APPEND ...)` to tell the build system where to look for them.
* **COMPONENTS**: By explicitly setting `set(COMPONENTS main 01_basic_test)`, we strictly limit what gets compiled. Instead of processing the hundreds of components in the full ESP-IDF framework, we only build what is strictly necessary. This significantly **reduces compilation time**.
* **project(test_sum)**: This command finalizes the project definition and gives the output binary its name.

### 2. Component Level: main/CMakeLists.txt

Inside the `main` folder of our test project, we treat our test logic as a component itself.

#### Key Logic:
* **idf_component_register**: This is the standard way to register the source files (`main.cpp`, `test_sum.cpp`) and include directories.
* **REQUIRES**: We explicitly declare that this test component depends on:
    1. **gtest**: Our external wrapper.
    2. **01_basic_test**: The actual logic we want to validate.
* **WHOLE_ARCHIVE**: This is a crucial flag for testing. GoogleTest uses static constructors to "auto-register" tests. However, since `test_sum.cpp` functions are never explicitly called by `main.cpp`, the linker might think they are unused and skip them to save space. `WHOLE_ARCHIVE` forces the linker to include every object file, ensuring all tests are discovered and executed.