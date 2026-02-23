# Test strategy: Sum component

This chapter adds tests for `add_constrained_err`, the new method that returns `esp_err_t` instead of `-1` on failure. The previous tests from [01_basic_test](../../01_basic_test/host_test/test_sum/README.md) are kept as-is.

---

## test_sum.cpp

### add_constrained_err — happy path

Valid inputs within bounds. Checks both the return code and the result via reference.

```cpp
int result = 0;
esp_err_t err = calc.add_constrained_err(3, 4, result);
EXPECT_EQ(err, ESP_OK);
EXPECT_EQ(result, 7);
```

### add_constrained_err — edge cases

Tests the exact boundaries: minimum (0 + 0) and maximum (5 + 5, 10 + 0).

```cpp
err = calc.add_constrained_err(5, 5, result);
EXPECT_EQ(err, ESP_OK);
EXPECT_EQ(result, 10);
```

### add_constrained_err — out of range

The function returns different error codes depending on what failed:

- Input outside 0–10 → `ESP_ERR_INVALID_ARG`
- Inputs valid but result exceeds 10 → `ESP_FAIL`

```cpp
// Input above limit
err = calc.add_constrained_err(11, 0, result);
EXPECT_EQ(err, ESP_ERR_INVALID_ARG);

// Valid inputs, invalid result (6 + 5 = 11)
err = calc.add_constrained_err(6, 5, result);
EXPECT_EQ(err, ESP_FAIL);
```

---

## test_sum_param.cpp

The same tests for `add_constrained_err`, written as parameterized tests using `TestWithParam`. For a function this simple the difference isn't dramatic — but when you have many input/output combinations to cover, this pattern avoids repeating the same test structure over and over. Worth seeing early.

Each case is a struct with inputs, expected result, and expected error:

```cpp
struct SumParams {
    int a;
    int b;
    int result;
    esp_err_t error;
};
```

All cases go into a single `INSTANTIATE_TEST_SUITE_P` block:

```cpp
INSTANTIATE_TEST_SUITE_P(
    SumParamTest, SumParamTest,
    ::testing::Values(
        SumParams{3, 4, 7,  ESP_OK},
        SumParams{5, 5, 10, ESP_OK},
        SumParams{11, 0, -1, ESP_ERR_INVALID_ARG},
        SumParams{6,  5, -1, ESP_FAIL}
        // ...
    ));
```

One test function runs for each entry. Adding a new scenario is just adding a line.
