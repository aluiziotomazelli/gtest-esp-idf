# Test strategy: Sum component

The tests from [02_idf_mocks](../../02_idf_mocks/host_test/test_sum/README.md) are kept as-is. This chapter adds `test_sum_boss.cpp`, which tests `SumBoss` in isolation using GMock.

---

## test_sum_boss.cpp

### MockSum

Before the tests, a mock is created from `ISum`:

```cpp
class MockSum : public ISum {
public:
    MOCK_METHOD(int, add, (int a, int b), (override));
    MOCK_METHOD(int, add_constrained, (int a, int b), (override));
    MOCK_METHOD(esp_err_t, add_constrained_err, (int a, int b, int &result), (override));
};
```

`SumBoss` receives an `ISum&` via constructor injection, so it has no idea whether it's talking to a real `Sum` or a mock. That's the point: with the SumBoss coor

### DelegatesToSumAndReturnsResult

Verifies that the boss passes the arguments to `Sum` correctly and returns both the result and the error code untouched. `SetArgReferee<2>` simulates what the real `Sum` would write into the reference argument:

```cpp
EXPECT_CALL(mock, add_constrained_err(3, 4, _))
    .WillOnce(DoAll(SetArgReferee<2>(7), Return(ESP_OK)));

EXPECT_EQ(ESP_OK, err);
EXPECT_EQ(7, result);
```

### PropagatesErrorFromSum

Verifies that when `Sum` returns an error, the boss passes it through unchanged. The result value is not checked — when an error occurs, its value is unspecified.

```cpp
EXPECT_CALL(mock, add_constrained_err(11, 5, _))
    .WillOnce(Return(ESP_ERR_INVALID_ARG));

EXPECT_EQ(ESP_ERR_INVALID_ARG, err);
```

These two tests are modest — SumBoss is still a thin layer with no logic of its own. What they validate is the contract: the boss delegates correctly and doesn't interfere with what Sum returns.