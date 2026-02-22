## Test Strategy: Sum Component

To ensure full coverage of our component, we implemented several testing tiers, ranging from environment validation to specific business rules.

### 1. Smoke Test (GTest Verification)
Before testing logic, we perform a "Smoke Test" to ensure the GoogleTest framework is correctly integrated with the ESP-IDF build system. If this fails, the issue is environmental, not logical.
```cpp
// Verifies that the gtest environment is operational
TEST(TestSum, GTestVerify) {
    EXPECT_TRUE(true);
}
```

### 2. Basic Addition (Standard Logic)
This tier validates the standard `add` function. It ensures that basic arithmetic—including positive numbers, negative numbers, and zero—behaves as expected without any range restrictions.
```cpp
// Verifies standard arithmetic without constraints
EXPECT_EQ(calc.add(2, 2), 4);
EXPECT_EQ(calc.add(-1, -1), -2);
```

### 3. Constrained Logic (add_constrained)
To ensure the reliability of the `add_constrained` function, we implemented three distinct testing tiers:

#### A. Happy Path
This test validates the function under ideal conditions. It confirms that when inputs are well within the specified boundaries, the logic performs the arithmetic correctly.
```cpp
// Example: Adding 3 and 4 returns 7
EXPECT_EQ(calc.add_constrained(3, 4), 7);
```

#### B. Edge Cases
Edge cases focus on the **boundaries** of our logic. In embedded systems, bugs often hide at the exact transition points. We test the minimum (0) and maximum (10) allowed values to ensure the logic is precise.
```cpp
// Example: Checking the upper limit of 10
EXPECT_EQ(calc.add_constrained(5, 5), 10);
```

#### C. Out of Range (Negative Testing)
Also known as "Negative Testing," this category ensures that the system handles invalid data gracefully. Instead of returning a corrupt sum, the function must return the designated error code (`-1`).
```cpp
// Example: Result 11 is invalid and must return -1
EXPECT_EQ(calc.add_constrained(6, 5), -1);
```

## Running the Tests

To execute the unit tests on your host machine (Linux), follow these steps from the component root:

### 1. Set the target to Linux
This command prepares the build system to compile using your local GCC/Clang instead of the Xtensa/RISC-V cross-compiler.
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