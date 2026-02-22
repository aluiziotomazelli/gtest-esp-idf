![Build Status](https://github.com/aluiziotomazelli/gtest-esp-idf/actions/workflows/01_build_esp32.yml/badge.svg)
![Host Tests Status](https://github.com/aluiziotomazelli/gtest-esp-idf/actions/workflows/01_host_tests.yml/badge.svg)

# 01_basic_test: structure and GTest wrapper

This folder is a standalone ESP-IDF component named `01_basic_test`. It covers two things: how to structure a component for both host testing and hardware deployment, and how to wrap GoogleTest so it plays nice with the IDF build system.

## Directory structure

```text
01_basic_test/
├── CMakeLists.txt             # Component registration
├── include/
│   ├── i_sum.hpp              # Interface (abstract base class)
│   └── sum.hpp                # Concrete class header
├── src/
│   └── sum.cpp                # Implementation
├── host_test/
│   ├── gtest/                 # GTest wrapper component
│   └── test_sum/              # Test project for the Sum class
└── test_apps/
    └── test_build/            # Minimal project to verify ESP32 compilation
```

---

## Component files

### include/i_sum.hpp

This file defines the interface using pure virtual functions. For this example it's not strictly needed — the `Sum` class is simple enough to test directly. But defining an interface from the start is worth it: in future tests, you can derive a **mock** from `i_sum.hpp` using GMock, which lets you test any component that depends on `Sum` without using the real implementation. Without an interface, mocking isn't possible.

### src/sum.cpp

The actual logic. This is what runs on the ESP32 in production.

### test_apps/test_build/

A minimal ESP-IDF project that verifies the component compiles for hardware:

```bash
idf.py set-target esp32
idf.py build
idf.py flash monitor
```

---

## The GTest wrapper

GoogleTest isn't part of ESP-IDF, so it needs to be introduced as a component. The wrapper lives at `host_test/gtest/` and is just a `CMakeLists.txt` — no source files.

### 1. Linux-only guard

```cmake
if(IDF_TARGET STREQUAL "linux")
```

GTest only gets processed when the build target is `linux`. This prevents it from being compiled into the ESP32 binary.

### 2. FetchContent

```cmake
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
```

GTest is downloaded from GitHub at build time. The repo stays small, and updating the version is just changing the tag.

### 3. Build phase guard

```cmake
if(NOT CMAKE_BUILD_EARLY_EXPANSION)
  FetchContent_MakeAvailable(googletest)
endif()
```

ESP-IDF runs in two phases: first it scans components to resolve dependencies, then it actually builds. FetchContent can't run during the scan phase — it would fail trying to download an external library while the build system is still mapping out what exists. The guard restricts the download to the real build phase.

### 4. INTERFACE linking

Since the wrapper has no source files of its own, it registers as an `INTERFACE` component. This means it doesn't compile anything itself — it just exposes GTest and GMock to whoever needs them. Any component that lists `gtest` in its `REQUIRES` gets access automatically.

Even in this example where only one test project uses GTest, the INTERFACE approach is the right pattern. It keeps the wrapper reusable as more test projects are added.

---

## Test project structure (test_sum)

`host_test/test_sum/` is a standalone ESP-IDF project configured to build for Linux. Two CMake details are worth noting:

### EXTRA_COMPONENT_DIRS

```cmake
list(APPEND EXTRA_COMPONENT_DIRS "../../" "../../host_test")
```

The component under test (`01_basic_test`) and the GTest wrapper are outside this project's folder. `EXTRA_COMPONENT_DIRS` tells the build system where to find them.

### COMPONENTS

```cmake
set(COMPONENTS main 01_basic_test)
```

Without this, the build would process the entire ESP-IDF component tree — hundreds of components. Listing only what's needed cuts compilation time significantly.

### WHOLE_ARCHIVE

```cmake
idf_component_register(
    ...
    WHOLE_ARCHIVE
)
```

GoogleTest registers tests through static constructors. Since `test_sum.cpp` functions are never called directly by `main.cpp`, the linker may discard them as unused. `WHOLE_ARCHIVE` forces every object file to be included, so all tests are discovered and run.

---

## Running the tests

From the test project folder:

```bash
cd 01_basic_test/host_test/test_sum
idf.py --preview set-target linux
idf.py build
./build/test_sum.elf
```

The `--preview` flag is needed because Linux target support is still marked as a preview feature in ESP-IDF. It switches the compiler from Xtensa/RISC-V to your local GCC.

For the test strategy and what each test covers, see the [test_sum README](host_test/test_sum/README.md).