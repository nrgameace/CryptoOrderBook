# CryptoOrderBook

A high-performance cryptocurrency order book and matching engine built in C++20. This system manages a real-time limit order book, matches buy and sell orders using price-time priority, and persists all executed trades to a SQLite database.

Designed with the same core principles used by production-grade exchanges: fixed-point integer arithmetic for deterministic precision, sorted price-level maps for efficient best-bid/best-ask retrieval, and time-priority queues for fair order execution. A producer-consumer simulator drives live order flow using real BTC prices fetched from the CoinMarketCap API.

---

## Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Project Structure](#project-structure)
- [Component Overview](#component-overview)
  - [Order](#order)
  - [OrderBook](#orderbook)
  - [MatchingEngine](#matchingengine)
  - [MarketStats](#marketstats)
  - [Simulator](#simulator)
  - [PriceGenerator](#pricegenerator)
  - [IPriceFetcher / CoinMarketCapFetcher](#ipricefetcher--coinmarketcapfetcher)
  - [TransactionLogger](#transactionlogger)
  - [Utils](#utils)
- [Data Structures and Design Decisions](#data-structures-and-design-decisions)
- [Build Instructions](#build-instructions)
- [Running the Application](#running-the-application)
- [Running Tests](#running-tests)
- [Benchmarks](#benchmarks)
- [Roadmap](#roadmap)
- [Technologies](#technologies)

---

## Features

- **Thread-Safe Order Book** -- Reader-writer locking via `std::shared_mutex`: concurrent reads share a lock, writes take an exclusive lock
- **Producer-Consumer Simulator** -- Dedicated producer thread generates orders; consumer thread drains a shared queue and runs the matching engine. Coordinated with `std::condition_variable` and `std::atomic<bool>` for clean shutdown
- **Live Price Feed** -- BTC price fetched from the CoinMarketCap Pro API on startup and cached with a 1-second TTL; `IPriceFetcher` interface allows the fetcher to be swapped in tests
- **Limit Order Book** -- Separate buy and sell sides, each organized by price level with time-priority queues at each level
- **Matching Engine** -- Continuously matches overlapping buy and sell orders, executing trades at the resting order's price
- **Rolling Market Statistics** -- `MarketStats` tracks VWAP, bid-ask spread, trade volume, and price volatility over a configurable rolling window using a ring buffer and Welford's online algorithm
- **Fixed-Point Integer Arithmetic** -- All internal price and quantity values use `int64_t` with a scale factor of 10^8, eliminating floating-point precision errors entirely
- **Trade Persistence** -- Every executed trade is logged to a SQLite database using parameterized prepared statements
- **Partial Fill Support** -- Orders that are not fully matched retain their remaining quantity and stay in the book for future matching
- **Benchmarking Suite** -- Standalone binary measuring `addOrder` latency, `executeTrade` latency, and `simulateMarket` throughput at N = 1 / 1k / 10k / 100k, reporting mean, p50, and p99

---

## Architecture

```
                    +---------------------------+
                    |           Main            |
                    +-------------+-------------+
                                  |
                    +-------------v-------------+
                    |         Simulator         |
                    |                           |
                    |  producerThread           |  consumerThread
                    +--------+------------------+--------+
                             |                           |
              +--------------v-----------+   +-----------v-----------+
              |       PriceGenerator     |   |    transition queue   |
              |  (normal dist, σ=15)     |   |   (std::queue<Order>) |
              +--------------+-----------+   +-----------+-----------+
                             |                           |
              +--------------v-----------+   +-----------v-----------+
              |       IPriceFetcher      |   |     MatchingEngine    |
              |  (CoinMarketCapFetcher)  |   |                       |
              |  BTC price, 1s TTL cache |   |  addOrder()           |
              +--------------------------+   |  simulateMarket()     |
                                             +----+-------------+----+
                                                  |             |
                              +-------------------v--+   +------v-------+
                              |       OrderBook      |   |  MarketStats |
                              |  (std::shared_mutex) |   | (ring buffer)|
                              |  buy: map desc price |   | VWAP, spread,|
                              |  sell: map asc price |   | vol, volatil.|
                              +-------------------+--+   +--------------+
                                                  |
                              +-------------------v--+
                              |  TransactionLogger   |
                              |  SQLite (TRADES)     |
                              +----------------------+
```

---

## Project Structure

```
CryptoOrderBook/
├── CMakeLists.txt
├── README.md
├── changelog.md
├── .env.example
├── include/
│   ├── CoinMarketCapFetcher.h
│   ├── dotenv.h
│   ├── IPriceFetcher.h
│   ├── MarketStats.h
│   ├── MatchingEngine.h
│   ├── MockTransactionLogger.h
│   ├── Order.h
│   ├── OrderBook.h
│   ├── PriceGenerator.h
│   ├── Simulator.h
│   ├── TradeRecord.h
│   ├── TransactionLogger.h
│   ├── TransactionLoggerInterface.h
│   └── Utils.h
├── src/
│   ├── CoinMarketCapFetcher.cpp
│   ├── Main.cpp
│   ├── MarketStats.cpp
│   ├── MatchingEngine.cpp
│   ├── MockTransactionLogger.cpp
│   ├── Order.cpp
│   ├── OrderBook.cpp
│   ├── PriceGenerator.cpp
│   ├── Simulator.cpp
│   ├── TransactionLogger.cpp
│   └── Utils.cpp
├── tests/
│   ├── TestMarketStats.cpp
│   ├── TestMatchingEngine.cpp
│   ├── TestOrder.cpp
│   ├── TestOrderBook.cpp
│   ├── TestPriceGenerator.cpp
│   ├── TestSimulator.cpp
│   ├── TestTransactionLogger.cpp
│   └── TestUtils.cpp
├── benchmarks/
│   └── Benchmarks.cpp
├── Database/
│   └── TransactionHistory.db
└── build/
```

---

## Component Overview

### Order

Represents a single buy or sell order in the market.

| Field | Type | Description |
|-------|------|-------------|
| `transactionSide` | `OrderType` | Buy or sell |
| `price` | `int64_t` | Price in fixed-point units (scaled by 10^8) |
| `quantity` | `int64_t` | Quantity in fixed-point units (scaled by 10^8) |
| `timestamp` | `time_point` | Automatically set at construction |
| `transactionId` | `int` | Unique identifier for the order |
| `userId` | `int` | Identifier of the user who placed the order |

The constructor accepts human-readable `double` values for price and quantity and converts them to `int64_t` internally. `operator<` compares by timestamp so `Order` can participate in a min-heap priority queue for time priority. `operator==` provides field-level equality for test assertions.

### OrderBook

Maintains two sorted maps representing the buy and sell sides of the market, protected by a `mutable std::shared_mutex` for safe concurrent access.

- **Buy side**: `std::map<int64_t, std::priority_queue<Order, ...>, std::greater<int64_t>>` -- Descending by price; highest bid at `begin()`
- **Sell side**: `std::map<int64_t, std::priority_queue<Order, ...>>` -- Ascending by price; lowest ask at `begin()`

Each price level maps to a priority queue ordered by `OrderTimePriority` (a custom comparator that puts earlier timestamps on top), ensuring FIFO execution within the same price level.

**Locking scheme**: read operations (`getBestBid`, `getBestAsk`, `isBuySideEmpty`, `isSellSideEmpty`, `getBuyDepth`, `getSellDepth`) take a `std::shared_lock`; write operations (`addOrder`, `removeBestBid`, `removeBestAsk`) take a `std::unique_lock`.

### MatchingEngine

The core trading logic. Holds a reference to an `OrderBook` and a `TransactionLoggerInterface`.

- **`processOrder(buy, sell)`** -- Validates sides, checks price overlap, determines the resting order (earlier timestamp), and delegates to `executeTrade()` at the resting price. Handles full matches, partial fills on either side.
- **`simulateMarket(MarketStats* stats = nullptr)`** -- Iterates: peek best bid and best ask, break if spread doesn't overlap, execute the trade, re-insert any partially filled remainder. If `stats` is non-null, records each trade and calls `printSummary()` after the loop. Returns `true` when both sides are empty.
- **`addOrder(order)`** -- Thin delegator to `book.addOrder()`, used by `Simulator`'s consumer thread.

### MarketStats

Computes rolling statistics from executed trades over a configurable window.

| Method | Description |
|--------|-------------|
| `recordTrade(price, qty)` | Appends a trade point to the ring buffer |
| `updateSpread(bid, ask)` | Updates the last-known best bid and ask |
| `getVWAP()` | Volume-weighted average price over the window |
| `getSpread()` | Current bid-ask spread in dollars |
| `getVolume()` | Sum of quantity traded within `windowSeconds` |
| `getVolatility()` | Sample standard deviation of trade prices via Welford's algorithm |
| `printSummary()` | Prints VWAP, spread, volume, and volatility to stdout |

Constructor parameters: `windowTrades` (ring buffer capacity, default 100) and `windowSeconds` (time-based volume/volatility window, default 60s). Throws `std::invalid_argument` if `windowTrades ≤ 0`.

### Simulator

Implements the producer-consumer architecture that drives continuous live order flow.

- **Producer thread** (`producerLoop`): constructs a `PriceGenerator`, generates 10 orders per batch, pushes them onto `transitionQueue`, sleeps 100 ms, repeats while `running`.
- **Consumer thread** (`consumerLoop`): waits on `empty` condition variable, drains the full queue into a local batch, calls `engine.addOrder()` for each order, then calls `engine.simulateMarket()`.
- **`start()`** / **`stop()`**: set `running` atomically, notify the condition variable, and join both threads. The destructor calls `stop()`.
- Copy and assignment are deleted; `running` is `std::atomic<bool>`.

### PriceGenerator

Generates randomized buy and sell orders centered around the current market price.

- Calls `IPriceFetcher::fetchPrice()` on each `generateOrders()` call to refresh the reference price.
- Prices drawn from `N(currentPrice, σ=15)` using `std::normal_distribution` and a seeded `std::mt19937`.
- Buy/sell side assigned uniformly at random. All orders are generated with a fixed quantity of 2.0.

### IPriceFetcher / CoinMarketCapFetcher

`IPriceFetcher` is a pure virtual interface with a single method `fetchPrice() -> double`. All components that need a price source depend on this interface, not on the concrete fetcher, enabling injection of a test double.

`CoinMarketCapFetcher` implements `IPriceFetcher`:
- Reads `COIN_MARKET_API_KEY` from the environment (loaded via `dotenv.h` from a `.env` file).
- Fetches BTC price from the CoinMarketCap Pro API (`/v1/cryptocurrency/listings/latest`).
- Caches the result for 1 second to avoid hammering the API on high-frequency calls.
- Throws `std::runtime_error` if the API key is missing or the request fails.

### TransactionLogger

Persists executed trades to a SQLite database. The `TRADES` table schema:

```sql
CREATE TABLE TRADES (
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    buyUserId  INTEGER,
    sellUserId INTEGER,
    quantity   REAL,
    price      REAL,
    timestamp  BIGINT
);
```

Internal `int64_t` values are converted to `double` at the database boundary for readability. Copy, move, and assignment are deleted — the logger owns the raw `sqlite3*` handle.

`MockTransactionLogger` implements `TransactionLoggerInterface` and records all calls as `TradeRecord` structs in a `std::vector`. Tests assert against `getRecord(index)` without touching SQLite.

### Utils

Shared fixed-point conversion functions used across all components.

| Symbol | Description |
|--------|-------------|
| `SCALE_FACTOR` | `constexpr int64_t = 100'000'000` (10^8) |
| `convertToInternal(double)` | Multiplies by `SCALE_FACTOR`, rounds to `int64_t` |
| `convertToDouble(int64_t)` | Divides by `SCALE_FACTOR`, returns `double` |

---

## Data Structures and Design Decisions

**Fixed-Point Arithmetic over Floating-Point**: Financial systems require deterministic arithmetic. Using `double` introduces precision errors that accumulate over thousands of trades (e.g., `0.1 + 0.2 != 0.3`). By scaling all values by 10^8 and storing them as `int64_t`, all internal comparisons and arithmetic are exact. The scale factor of 10^8 supports precision down to 1 satoshi while allowing values up to approximately 92 billion before overflow.

**Sorted Maps for Price Levels**: `std::map` provides O(log n) insertion and lookup with guaranteed ordering. `std::greater` on the buy side ensures the highest bid is at `begin()` in O(1). The sell side uses ascending order so the lowest ask is at `begin()`.

**Priority Queues for Time Priority**: Within a single price level, a `std::priority_queue<Order, ..., OrderTimePriority>` using a custom comparator (earlier timestamp = higher priority) ensures the earliest-placed order is matched first, implementing fair FIFO execution within each price level.

**Resting Order Price Execution**: When two orders match, the trade executes at the price of the resting order (the one already in the book when the incoming order arrived). This is the standard convention used by major exchanges and gives incoming orders price improvement when the spread overlaps.

**Dependency Injection via Interfaces**: `MatchingEngine` depends on `TransactionLoggerInterface`, not `TransactionLogger` directly. `PriceGenerator` and `Simulator` depend on `IPriceFetcher`, not `CoinMarketCapFetcher`. This means the SQLite database and the HTTP client are never involved in unit tests — `MockTransactionLogger` and a stub `IPriceFetcher` are injected instead.

**`std::shared_mutex` for Read-Heavy Order Book Access**: Most order book operations in a running simulation are reads (checking best bid/ask, depth queries). `std::shared_mutex` allows multiple reader threads to hold a shared lock simultaneously, blocking only when a write lock is needed. This avoids unnecessary serialization of concurrent reads.

**Condition Variable + Atomic Shutdown Pattern**: The consumer thread blocks on a `std::condition_variable` rather than busy-waiting. The producer notifies after each batch. `stop()` sets `running` to `false`, calls `notify_all()`, and joins both threads — guaranteeing the consumer drains any remaining orders before exit.

---

## Build Instructions

### Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- CMake 3.16 or higher
- SQLite3 development libraries

### Environment Setup

Copy `.env.example` to `.env` in the project root and fill in your CoinMarketCap API key:

```bash
cp .env.example .env
# edit .env and set COIN_MARKET_API_KEY=<your key>
```

The `.env` file must be one directory above the build directory (i.e., in the project root) because `CoinMarketCapFetcher` loads it with `dotenv::init("../.env")` relative to the working directory of the running binary.

### Build

```bash
mkdir -p build && cd build
cmake ..
make
```

CMake will automatically fetch and build the following dependencies via FetchContent:

| Dependency | Version | Purpose |
|------------|---------|---------|
| Google Test | v1.14.0 | Unit testing framework |
| cpp-httplib | v0.41.0 | HTTP client for CoinMarketCap API requests |
| nlohmann/json | v3.11.3 | JSON parsing for API responses |

`dotenv.h` is vendored directly in `include/` and requires no separate fetch step.

---

## Running the Application

From the `build/` directory:

```bash
./main.out
```

This starts the producer-consumer simulator:
- The producer thread fetches the live BTC price from CoinMarketCap and generates batches of 10 orders every 100 ms.
- The consumer thread matches orders and logs executed trades to `Database/TransactionHistory.db`.

Press **Enter** to stop the simulator cleanly. The stopper thread calls `simulator.stop()`, which sets the shutdown flag, wakes the consumer, and joins both threads before the process exits.

---

## Running Tests

From the `build/` directory:

```bash
./all_tests
```

The test suite covers:

| Test Suite | Coverage |
|------------|----------|
| **TestOrder** | Field access, timestamp accuracy, `operator<` for priority queue ordering, `operator==` |
| **TestOrderBook** | `addOrder`, depth queries across multiple price levels, buy/sell separation, `removeBestBid`/`removeBestAsk`, empty book behavior |
| **TestMatchingEngine** | Constructor, `processOrder` (full match, partial fills, non-overlapping), `simulateMarket` (multiple scenarios) |
| **TestTransactionLogger** | `logTrade` persistence via `MockTransactionLogger` |
| **TestUtils** | `convertToInternal`, `convertToDouble`, round-trip accuracy, fractional values, negative values, zero |
| **TestMarketStats** | VWAP, spread, volume, volatility against hand-computed values; window eviction; invalid constructor arguments |
| **TestPriceGenerator** | Order count, side distribution, fetcher called on generate, regenerate clears previous orders, zero-order edge case |
| **TestSimulator** | Constructor state, `start`/`stop` lifecycle |

---

## Benchmarks

`benchmarks/Benchmarks.cpp` contains a standalone benchmark binary built as `./benchmarks`. It measures three operations at N = 1 / 1,000 / 10,000 / 100,000:

| Benchmark | What it measures |
|-----------|-----------------|
| `benchmarkAddOrder` | Per-call latency of `OrderBook::addOrder()` — map insertion and priority queue push |
| `benchmarkExecuteTrade` | Per-call latency of `MatchingEngine::processOrder()` — trade execution path only, no book insertion |
| `benchmarkSimulateMarket` | Total time and throughput (orders/sec) of `simulateMarket()` over a pre-loaded book |

Each benchmark reports total time, mean latency, p50, and p99 in nanoseconds. Run with compiler optimizations (`cmake -DCMAKE_BUILD_TYPE=Release ..`) for representative numbers.

See `BENCHMARKS.md` for recorded results.

---

## Roadmap

- [ ] Phase 2 optimization pass: profile with `perf`/`callgrind`, investigate flat containers or `unordered_map` + separate best-price tracker, document before/after measurements in `BENCHMARKS.md`
- [ ] Order cancellation: lazy deletion via `std::unordered_map<int, OrderStatus>`, skip cancelled orders at match time (Phase 5)
- [ ] Market order support: execute immediately at best available price, no resting in the book
- [ ] Order book depth snapshots and L2 data output
- [ ] REST or WebSocket API for external order submission

---

## Technologies

| Technology | Purpose |
|------------|---------|
| **C++20** | Core language (structured bindings, `constexpr`, `chrono`, `shared_mutex`, `atomic`) |
| **CMake** | Cross-platform build system with FetchContent dependency management |
| **SQLite3** | Lightweight embedded database for trade persistence |
| **Google Test** | Unit testing framework (v1.14.0, fetched at build time) |
| **cpp-httplib** | Header-only HTTP client for CoinMarketCap API requests (v0.41.0) |
| **nlohmann/json** | Header-only JSON library for API response parsing (v3.11.3) |
| **dotenv.h** | Header-only `.env` file loader for API key management (vendored) |
