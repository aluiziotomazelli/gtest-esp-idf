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


