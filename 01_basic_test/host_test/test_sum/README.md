# Test strategy: Sum component

The `Sum` class has two methods: `add(a, b)` for plain addition, and `add_constrained(a, b)` which returns `-1` if the result exceeds the allowed range. The tests are split into three groups.

## 1. Smoke test

Checks that GoogleTest itself is running. If this fails, something is wrong with the setup, not the code.

```cpp
TEST(TestSum, GTestSmokeTest) {
    EXPECT_TRUE(true);
}
```

## 2. Basic addition

Covers `add(a, b)` with positive numbers, negative numbers, and zero.

```cpp
EXPECT_EQ(calc.add(2, 2), 4);
EXPECT_EQ(calc.add(-1, -1), -2);
EXPECT_EQ(calc.add(0, 0), 0);
```

## 3. Constrained addition

Three scenarios for `add_constrained`:

**Happy path** — inputs well within bounds, function returns the sum normally.

```cpp
EXPECT_EQ(calc.add_constrained(3, 4), 7);
```

**Edge cases** — tests the exact boundary values. In embedded systems, bugs often hide at transition points, so testing the minimum (0) and maximum (10) explicitly matters.

```cpp
EXPECT_EQ(calc.add_constrained(5, 5), 10);  // exactly at the limit
EXPECT_EQ(calc.add_constrained(0, 0), 0);   // minimum
```

**Out of range** — when the result exceeds the limit, the function must return `-1` instead of a corrupt sum.

```cpp
EXPECT_EQ(calc.add_constrained(6, 5), -1);  // 11 > 10
```