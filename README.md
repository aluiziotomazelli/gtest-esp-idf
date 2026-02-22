# GoogleTest Integration (The Wrapper Approach)

Unlike **Unity** (which is a native component integrated into ESP-IDF), **GoogleTest** must be "wrapped" to work within the IDF build system. This is achieved through the configuration in `01_basic_test/host_test/gtest/CMakeLists.txt`.

## Understanding ESP-IDF Components
In the ESP-IDF ecosystem, every library or module is treated as a **Component**. Even a simple project like "Blink" has a `main` component. For more details on this architecture, refer to the [official documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/build-system.html) or this [YouTube - technical deep dive](https://www.youtube.com/watch?v=7utLBxSOXlQ&t=2484s).

To make GTest available, we create a "wrapper component" that handles the framework's lifecycle.

## Key Implementation Details

### 1. Target Validation
The script first ensures that GTest is only processed when the build target is `linux`. Since GTest is intended for host-based testing, we prevent it from being included in hardware builds (e.g., ESP32), which avoids compilation errors and saves resources.

### 2. The FetchContent Strategy
Instead of manually downloading or including the GTest source code in our repository, we use the `FetchContent` module:
* **Lightweight Repository**: The component remains small because the framework is only downloaded during the build phase.
* **Easy Version Control**: Switching GTest versions is as simple as updating the URL or tag in the `FetchContent_Declare` block.
* **Automation**: `FetchContent_MakeAvailable` automatically handles the download and integration of the library.

### 3. Handling the Two-Phase Build System
The ESP-IDF build system operates in two distinct stages. This is why we use the guard `if(NOT CMAKE_BUILD_EARLY_EXPANSION)`:
* **Requirement Expansion Phase**: IDF first runs a script to discover dependencies and build the component tree.
* **Actual Build Phase**: Once dependencies are resolved, the real compilation starts.
Wrapping the logic in this guard ensures that heavy tasks—like downloading GTest and processing its internal CMake rules—only happen during the **actual build phase**. This prevents the initial dependency discovery script from failing or hanging while trying to fetch external content.

### 4. Interface Linking
Since our `gtest` wrapper folder doesn't contain source files (it only fetches an external library), we use `target_link_libraries` with the `INTERFACE` keyword. This links the newly created `gtest` and `gmock` targets to the IDF component library, making them available to any other component that lists `gtest` in its `REQUIRES` or `PRIV_REQUIRES` list.

## Running the Tests

To execute the unit tests on your host machine (Linux), follow these steps from the component root:

### 1. Navigate to the test project
```bash
cd 01_basic_test/host_test/test_sum
```

### 2. Set the target to Linux
We use the `--preview` flag because host-based testing is an evolving feature in ESP-IDF. This command prepares the build system to compile using your local GCC/Clang instead of the Xtensa/RISC-V cross-compiler.
```bash
idf.py --preview set-target linux
```

### 3. Build the project
This will trigger the GTest download (via FetchContent) and compile both the component and the test suite.
```bash
idf.py build
```

### 4. Execute the binary
Once the build is complete, the executable ELF file will be located in the `build` directory. Run it directly to see the GoogleTest output:
```bash
./build/test_sum.elf
```

---

## Expected Output
If everything is correctly configured, you should see an output similar to this:
```text
[==========] Running 6 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 6 tests from TestSum
[ RUN      ] TestSum.GTestSmokeTest
[       OK ] TestSum.GTestSmokeTest (0 ms)
[ RUN      ] TestSum.BasicAddition
[       OK ] TestSum.BasicAddition (0 ms)
[ RUN      ] TestSum.NegativeAddition
[       OK ] TestSum.NegativeAddition (0 ms)
[ RUN      ] TestSum.AddConstrained_HappyPath
[       OK ] TestSum.AddConstrained_HappyPath (0 ms)
[ RUN      ] TestSum.AddConstrained_EdgeCases
[       OK ] TestSum.AddConstrained_EdgeCases (0 ms)
[ RUN      ] TestSum.AddConstrained_OutOfRange
[       OK ] TestSum.AddConstrained_OutOfRange (0 ms)
[----------] 6 tests from TestSum (0 ms total)

[----------] Global test environment tear-down
[==========] 6 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 6 tests.
```
