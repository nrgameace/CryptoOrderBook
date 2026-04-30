# Changelog

All notable changes to CryptoOrderBook are documented here.
Entries are derived strictly from commit diffs and tagged ranges.

## [Unreleased]

No commits after v1.0.

---

## [v0.1] — Initial implementation — 2026-02-23

### Added
- `Order` struct with price, quantity, side (buy/sell), and timestamp (`include/Order.h`)
- `OrderBook` class with `addOrder`, `cancelOrder`, `processOrder`, and `simulateMarket` (`include/OrderBook.h`, `src/OrderBook.cpp`)
- SQL-backed `TransactionLogger` for recording completed trades (`src/OrderBook.cpp`, `9fc1682`)
- `simulateMarket` function to replay order sequences against the book (`41a587f`)
- Initial test scaffolding in `tests/TestOrder.cpp` (`4e332d1`)
- `src/` and `include/` directory structure; headers relocated from project root (`4533d3f`, `7685424`)

### Fixed
- Bug in `addOrder` that caused incorrect order insertion (`dae7fde`)
- `simulateMarket` crashed when attempting to mutate an existing order — resolved by treating placed orders as immutable (`a5f0da7`)

### Design Decisions
- Headers separated into `include/` immediately to follow conventional C++ project layout rather than co-locating them with sources (`4533d3f`)

---

## [v0.2] — Architecture refactor — 2026-04-04

### Added
- `MatchingEngine` class extracted from `OrderBook` into its own `include/MatchingEngine.h` / `src/MatchingEngine.cpp` (`219936c`)
- `TransactionLoggerInterface` — pure abstract interface for the logger (`include/TransactionLoggerInterface.h`, `2937c7e`)
- `MockTransactionLogger` — test double implementing `TransactionLoggerInterface` (`include/MockTransactionLogger.h`, `src/MockTransactionLogger.cpp`, `24c6592`)
- `TradeRecord` struct to represent a completed trade (`include/TradeRecord.h`, `2937c7e`)
- `Utils.h` / `src/Utils.cpp` — shared helper functions (`f44f5bb`)
- Google Test suite via CMake `FetchContent`: `TestMatchingEngine`, `TestOrderBook`, `TestTransactionLogger`, `TestOrder` (`1ae6176` onward)
- Equality operator on `OrderBook` to support test assertions (`7e703b3`)
- OrderBook encapsulation pass — internal representation hidden behind public interface (`ecf1805`, `7c4771b`)

### Fixed
- Bug in `processOrder` method causing incorrect match behavior (`de7471f`)
- Prices changed from `double` to `int` to eliminate floating-point precision errors in financial calculations (`f44f5bb`)

### Design Decisions
- `MatchingEngine` extracted from `OrderBook` so matching logic and book management can be tested in isolation from each other (`219936c`)
- `TransactionLoggerInterface` introduced to enable constructor injection — `MockTransactionLogger` swaps in for tests without touching the database (`2937c7e`, `24c6592`)
- Prices stored as integers (smallest currency unit) rather than `double` to avoid rounding errors that compound across many trades (`f44f5bb`)

---

## [v0.3] — Benchmarking — 2026-04-06

### Added
- `benchmarks/Benchmarks.cpp` — standalone harness measuring order book throughput (`4a8ca7e`)
- `CMakeLists.txt` updated with a dedicated `benchmarks` build target (`4a8ca7e`)

### Design Decisions
- Benchmarks built as a separate CMake target so they run independently of the test suite and do not appear in `ctest` output (`4a8ca7e`)

---

## [v0.4] — Quant features — 2026-04-15

### Added
- `PriceGenerator` — generates synthetic price sequences for simulation (`include/PriceGenerator.h`, `src/PriceGenerator.cpp`, `08aef24`, `2926a50`)
- `dotenv.h` header for loading API credentials from environment files without hardcoding them (`2926a50`)
- `MarketStats` — computes market statistics over the order book (`include/MarketStats.h`, `src/MarketStats.cpp`, `a847fd1`)
- `tests/TestMarketStats.cpp` — test coverage for `MarketStats` (`a847fd1`)
- `src/Main.cpp` extended to exercise `PriceGenerator` and `MarketStats` (`08aef24`, `a847fd1`)

### Design Decisions
- `dotenv.h` used to keep API keys out of source control while still supporting local development with live credentials (`2926a50`)

---

## [v1.0] — Concurrency — 2026-04-30

### Added
- `Simulator` class — drives concurrent order book simulation using `jthread`, `shared_mutex`, and `condition_variable_any` (`include/Simulator.h`, `src/Simulator.cpp`, `cf01e6c`, `24f08a1`)
- `CoinMarketCapFetcher` — fetches live price data from the CoinMarketCap API (`include/CoinMarketCapFetcher.h`, `src/CoinMarketCapFetcher.cpp`, `2646c2e`)
- `IPriceFetcher` interface — abstraction over real and synthetic price sources (`include/IPriceFetcher.h`, `2646c2e`)
- API rate limiter in `MatchingEngine` and `Simulator` to respect CoinMarketCap request quotas (`cda7096`)
- Tests: `TestSimulator.cpp`, `TestPriceGenerator.cpp`, `TestUtils.cpp` (`2646c2e`)
- `CONCURRENCY.md` — documents the threading model, synchronization primitives, and design rationale (`0255b9c`–`7451b8b`)
- `README.md` expanded with full project documentation (`4077d2b`)
- `benchmarks/Benchmarks.cpp` updated to cover the concurrent simulation path (`32cdb3f`)

### Fixed
- Database corruption bug in `Simulator` triggered under concurrent writes (`379e130`)

### Design Decisions
- `jthread` chosen over `std::thread` so threads join automatically on `Simulator` destruction, preventing resource leaks on exceptions or early exit (`cf01e6c`)
- `shared_mutex` used so multiple reader threads can query market data simultaneously; order insertions take exclusive write locks (`cf01e6c`)
- `condition_variable_any` preferred over `condition_variable` because it is compatible with `shared_lock`, which `condition_variable` is not — required by the reader/writer pattern here (`cf01e6c`)
- `IPriceFetcher` interface introduced so `Simulator` accepts both `CoinMarketCapFetcher` (live) and `PriceGenerator` (synthetic), keeping concurrency logic price-source-agnostic (`2646c2e`)
- Rate limiter placed in `MatchingEngine` rather than only at the fetch layer so burst order submission is capped end-to-end, not just at the API boundary (`cda7096`)

---

[v1.0]: https://github.com/nrgameace/CryptoOrderBook/releases/tag/v1.0
[v0.4]: https://github.com/nrgameace/CryptoOrderBook/releases/tag/v0.4
[v0.3]: https://github.com/nrgameace/CryptoOrderBook/releases/tag/v0.3
[v0.2]: https://github.com/nrgameace/CryptoOrderBook/releases/tag/v0.2
[v0.1]: https://github.com/nrgameace/CryptoOrderBook/releases/tag/v0.1
