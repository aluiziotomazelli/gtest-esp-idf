# GoogleTest with ESP-IDF

This repo is a series of tutorials on how to use GoogleTest (GTest) for unit testing ESP-IDF components on Linux, without flashing to hardware.

## Why host-based testing?

The usual ESP-IDF loop is: edit, flash, read serial, repeat. For logic-heavy code — state machines, calculations, protocol parsing — this gets slow. Host-based testing lets you run your tests on Linux using your local GCC instead of the Xtensa/RISC-V cross-compiler. Feedback goes from minutes to seconds.

## Why GoogleTest instead of Unity?

ESP-IDF ships with Unity, a lightweight test framework that works well for basic assertions. The reason to bring in GoogleTest is **mocking**. GMock, which comes bundled with GTest, lets you create mock objects from interfaces. This makes it possible to test a component in isolation, without needing its real dependencies — a sensor, a driver, a network layer — to be present.

Unity works alongside CMock, which ESP-IDF uses internally to mock its own components. The problem is that CMock doesn't handle C++ classes — so if your component has multiple classes with distinct responsibilities, you end up writing mocks by hand. That gets tedious fast. GMock handles this directly in the test file, in a few lines.

## How ESP-IDF components work

In ESP-IDF, every library or module is a **component**: a folder with a `CMakeLists.txt` that registers its sources and dependencies using `idf_component_register`. The folder name is the component name. When one component needs another, it declares it in `REQUIRES`. The build system handles the rest.

Each tutorial in this repo is a standalone component that can be tested on Linux and also compiled for hardware.

## Repository structure

```text
gtest-esp-idf/
├── 01_basic_test/    # GTest wrapper setup and basic arithmetic tests
├── 02_mocks/         # (coming soon) Testing with GMock
└── ...
```

## Prerequisites

- **ESP-IDF 5.x** installed and sourced in your shell (`source $IDF_PATH/export.sh`)
- **Linux** or **WSL2** (the linux target doesn't work on macOS or native Windows)
- **CMake** (comes with ESP-IDF)
- **Git** (needed by FetchContent to download GTest)

## Running the first example

```bash
cd 01_basic_test/host_test/test_sum
idf.py --preview set-target linux
idf.py build
./build/test_sum.elf
```

For a full walkthrough of what each step does, see the [01_basic_test README](01_basic_test/README.md).

## Tutorial series

| Chapter                                  | Topic                                                 |
| ---------------------------------------- | ----------------------------------------------------- |
| [01_basic_test](01_basic_test/README.md) | GTest wrapper setup, component structure, basic tests |
| 02_mocks                                 | Testing with GMock (coming soon)                      |