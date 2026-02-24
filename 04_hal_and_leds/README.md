![Build Status](https://github.com/aluiziotomazelli/gtest-esp-idf/actions/workflows/03_build_esp32.yml/badge.svg)
![Host Tests Status](https://github.com/aluiziotomazelli/gtest-esp-idf/actions/workflows/03_host_tests.yml/badge.svg)

# 03_class_mock: testing a class that depends on another

This chapter builds on [02_idf_mocks](../02_idf_mocks/README.md). The GTest wrapper, CMake setup, and `esp_err_t` integration are explained there — this README covers only what changed.

The goal here is to introduce `SumBoss`, a class that depends on `ISum`, and test it in isolation using GMock. This is where having `ISum` as an interface since `01_basic_test` starts to matter.

---

## What changed

### include/sum_boss.hpp

The `SumBoss` class receives an `ISum` reference via constructor injection:

```cpp
class SumBoss {
public:
    SumBoss(ISum &sum);
    esp_err_t compute(int a, int b, int &result);
private:
    ISum &sum_;
};
```

Dependency injection is what makes the class testable in isolation — in production you pass a real `Sum`, in tests you pass a mock.

### src/sum_boss.cpp

```cpp
SumBoss::SumBoss(ISum &sum) : sum_(sum) {}

esp_err_t SumBoss::compute(int a, int b, int &result)
{
    esp_err_t ret = sum_.add_constrained_err(a, b, result);
    return ret;
}
```

For now `SumBoss` is a thin layer — it delegates to `Sum` and returns the result untouched. The boss is thin for now — that changes in the next chapter when it starts making decisions based on the result.

### CMakeLists.txt (component root)

`sum_boss.cpp` added to `SRCS`.

---

## test_sum_boss.cpp

Two tests cover `SumBoss`. They won't catch complex logic bugs — the boss is still a passthrough — but they validate the contract between the two classes and demonstrate the GMock mechanics that will matter more in the next chapter.

**DelegatesToSumAndReturnsResult** — verifies that the boss passes the arguments to `Sum` and returns both the result and the error code untouched. `DoAll(SetArgReferee<2>(...))` sets the output argument via reference, simulating what the real `Sum` would do:

```cpp
EXPECT_CALL(mock, add_constrained_err(3, 4, _))
    .WillOnce(DoAll(SetArgReferee<2>(7), Return(ESP_OK)));

EXPECT_EQ(ESP_OK, err);
EXPECT_EQ(7, result);
```

**PropagatesErrorFromSum** — verifies that when `Sum` returns an error, the boss propagates it instead of swallowing it:

```cpp
EXPECT_CALL(mock, add_constrained_err(6, 5, _))
    .WillOnce(Return(ESP_FAIL));

EXPECT_EQ(ESP_FAIL, err);
```
The tests are modest — the boss doesn't do much yet. But the real value shows up as the boss grows. In this example Sum is a simple arithmetic class, but imagine it were an ultrasonic sensor measuring distance. Instead of having a real sensor wired to a board, you can mock it and simulate any distance or response time you want — valid readings, out-of-range values, timeouts — and verify that the boss handles every scenario correctly. No hardware. No board. No waiting.

---

## Seeing it on real hardware

`test_apps/test_build/` has a working example with `SumBoss` integrated, using dependency injection with the real `Sum`. Valid and invalid inputs are tested and the results printed via serial:

```bash
cd 03_class_mock/test_apps/test_build
idf.py set-target esp32
idf.py build
idf.py flash monitor
```

---

## Running the tests

```bash
cd 03_class_mock/host_test/test_sum
idf.py --preview set-target linux
idf.py build
./build/test_sum.elf
```
Expected:
```text
[==========] Running 19 tests from 3 test suites.
[----------] Global test environment set-up.
[----------] 9 tests from TestSum
...
[----------] 2 tests from SumBossTest
...
[==========] 19 tests from 3 test suites ran. (0 ms total)
[  PASSED  ] 19 tests.
```

More about tests on its own [README](/host_test/test_sum/README.md)