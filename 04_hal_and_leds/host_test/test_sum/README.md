# Test strategy: Sum component

The tests from [03_class_mock](../../03_class_mock/host_test/test_sum/README.md) are kept as-is. This chapter adds two new test files: `test_led_sargent.cpp` and an updated `test_sum_boss.cpp`.

---

## test_led_sargent.cpp

`LedSargent` is tested in isolation using `MockGpioHal`. What's worth highlighting is what a mock makes possible here:

**ConstructorSetsCorrectPins** — verifies that the constructor calls `pin_set_direction` for both pins with the correct mode. On a real board, if this fails silently, the LEDs simply don't work with no obvious error.

**GreenSetsCorrectPin / RedSetsCorrectPin** — verifies that each method drives the correct pin high. A bug that inverts the pins would be invisible on a board unless you're watching both LEDs carefully.

**OffSetsCorrect** — verifies that both pins go low.

**OffAbortsOnFirstFailure / OffAbortsOnSecondFailure** — verifies the error propagation logic in `off()`. If the first pin fails, the second must not be touched. If the second fails, the error must propagate. Simulating a GPIO failure on a real board is nearly impossible — with a mock, it's one line.

The constructor test uses a regular `MockGpioHal` to be strict about unexpected calls. The method tests use `NiceMock<MockGpioHal>` to silence the constructor's `pin_set_direction` calls, so each test only deals with what it's actually testing.

---

## test_sum_boss.cpp

`SumBoss` now receives two dependencies — `ISum&` and `ILedSargent&` — so both are mocked in the unit tests:

**CallsGreenOnSuccess** — when `Sum` returns `ESP_OK`, the boss must call `green()` and not `red()`.

**CallsRedOnError** — when `Sum` returns an error, the boss must call `red()` and not `green()`. The sum error must propagate unchanged — the LED error is ignored by design.

**ReturnsLedErrorIfGreenFails** — when the sum succeeds but `green()` fails, the boss must return the LED error. This scenario would be nearly impossible to reproduce on hardware, but with mocks it's straightforward.

**PropagatesRedIgnoresLedError** — when both the sum and `red()` fail, the sum error is what reaches the caller.

### Integration tests

Three tests use the real `Sum` with a mocked `LedSargent`. These verify that the boss behaves correctly end-to-end with actual arithmetic — not a `WillOnce(Return(...))`:

- `CallsGreenOnSuccess` — `compute(3, 4)` with real `Sum`, expects `ESP_OK` and result `7`
- `CallsRedOnError` — `compute(6, 6)` exceeds the limit, expects `ESP_FAIL`
- `CallsRedWithInvalidArg` — `compute(-3, 4)` has an invalid input, expects `ESP_ERR_INVALID_ARG`

The LED is still mocked in all three — there's no hardware in host tests. What changes is that the error codes now come from the real `Sum`, not from a `WillOnce(Return(...))`.
