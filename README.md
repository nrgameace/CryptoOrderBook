# CryptoOrderBook

A high-performance cryptocurrency order book and matching engine built in C++20. This system manages a real-time limit order book, matches buy and sell orders using price-time priority, and persists all executed trades to a SQLite database.

Designed with the same core principles used by production-grade exchanges: fixed-point integer arithmetic for deterministic precision, sorted price-level maps for efficient best-bid/best-ask retrieval, and time-priority queues for fair order execution.

---

## Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Project Structure](#project-structure)
- [Component Overview](#component-overview)
  - [Order](#order)
  - [OrderBook](#orderbook)
  - [MatchingEngine](#matchingengine)
  - [TransactionLogger](#transactionlogger)
  - [Utils](#utils)
- [Data Structures and Design Decisions](#data-structures-and-design-decisions)
- [Build Instructions](#build-instructions)
- [Running the Application](#running-the-application)
- [Running Tests](#running-tests)
- [Roadmap](#roadmap)
- [Technologies](#technologies)

---

## Features

- **Limit Order Book** -- Maintains separate buy and sell sides, each organized by price level with time-priority queues at each level
- **Matching Engine** -- Continuously matches overlapping buy and sell orders, executing trades at the resting order's price (the order that was placed first)
- **Fixed-Point Integer Arithmetic** -- All internal price and quantity values use `int64_t` with a scale factor of 10^8, eliminating floating-point precision errors entirely
- **Trade Persistence** -- Every executed trade is logged to a SQLite database using parameterized prepared statements to prevent SQL injection
- **Partial Fill Support** -- Orders that are not fully matched retain their remaining quantity and stay in the book for future matching
- **Price-Time Priority** -- Orders are matched first by best price, then by earliest timestamp within the same price level
- **Comprehensive Test Suite** -- Unit tests covering order construction, order book operations, matching engine logic, and multi-scenario market simulations using Google Test

---

## Architecture

```
                    +------------------+
                    |    Main / API    |
                    | (Order Creation) |
                    +--------+---------+
                             |
                     addOrder(order)
                             |
                             v
                    +------------------+
                    |    OrderBook     |
                    |                  |
                    |  Buy Side (Map)  |
                    |  Sell Side (Map) |
                    +--------+---------+
                             |
                    simulateMarket()
                             |
                             v
                    +------------------+
                    |  MatchingEngine  |
                    |                  |
                    |  processOrder()  |
                    |  simulateMarket()|
                    +--------+---------+
                             |
                       logTrade()
                             |
                             v
                  +--------------------+
                  | TransactionLogger  |
                  |                    |
                  |  SQLite Database   |
                  |  (TRADES table)    |
                  +--------------------+
```

---

## Project Structure

```
CryptoOrderBook/
├── CMakeLists.txt
├── README.md
├── changelog.md
├── include/
│   ├── Order.h
│   ├── OrderBook.h
│   ├── MatchingEngine.h
│   ├── TransactionLogger.h
│   └── Utils.h
├── src/
│   ├── Main.cpp
│   ├── Order.cpp
│   ├── OrderBook.cpp
│   ├── MatchingEngine.cpp
│   ├── TransactionLogger.cpp
│   └── Utils.cpp
├── tests/
│   ├── TestOrder.cpp
│   ├── TestOrderBook.cpp
│   ├── TestMatchingEngine.cpp
│   └── TestTransactionLogger.cpp
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

The constructor accepts human-readable `double` values for price and quantity and converts them to `int64_t` internally. Operator overloads support timestamp-based comparison for priority queue ordering and field-level equality for testing.

### OrderBook

Maintains two sorted maps representing the buy and sell sides of the market.

- **Buy side**: `std::map<int64_t, std::priority_queue<Order>, std::greater<int64_t>>` -- Sorted in descending order by price so the highest bid is always at `begin()`
- **Sell side**: `std::map<int64_t, std::priority_queue<Order>>` -- Sorted in ascending order by price so the lowest ask is always at `begin()`

Each price level maps to a priority queue of orders sorted by timestamp, ensuring time priority within the same price.

### MatchingEngine

The core trading logic. Takes a reference to an `OrderBook` and processes orders until no more matches exist.

- **processOrder()** -- Matches a single buy/sell pair. Determines which order was resting (placed earlier) and executes the trade at the resting order's price. Handles full matches, partial fills on the buy side, and partial fills on the sell side.
- **simulateMarket()** -- Iteratively matches the best bid against the best ask until the spread no longer overlaps or one side of the book is empty. Returns `true` if the entire book is cleared.

All arithmetic uses exact integer operations with zero epsilon tolerance.

### TransactionLogger

Persists executed trades to a SQLite database. The `TRADES` table schema:

```sql
CREATE TABLE TRADES (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    buyUserId INTEGER,
    sellUserId INTEGER,
    quantity REAL,
    price REAL,
    timestamp BIGINT
);
```

Internal `int64_t` values are converted back to human-readable `double` values at the database boundary. The database schema stores doubles for readability and compatibility with external reporting tools.

### Utils

Shared utility header providing fixed-point conversion functions used across all components.

- `SCALE_FACTOR` -- `constexpr int64_t` equal to 100,000,000 (10^8)
- `convertToInternal(double)` -- Converts a human-readable double to its fixed-point `int64_t` representation
- `convertToDouble(int64_t)` -- Converts a fixed-point `int64_t` back to a human-readable double

---

## Data Structures and Design Decisions

**Fixed-Point Arithmetic over Floating-Point**: Financial systems require deterministic arithmetic. Using `double` introduces precision errors that accumulate over thousands of trades (e.g., `0.1 + 0.2 != 0.3`). By scaling all values by 10^8 and storing them as `int64_t`, all internal comparisons and arithmetic are exact. The scale factor of 10^8 supports precision down to 1 satoshi (the smallest unit of Bitcoin) while allowing values up to approximately 92 billion before overflow.

**Sorted Maps for Price Levels**: `std::map` provides O(log n) insertion and lookup with guaranteed ordering. Using `std::greater` for the buy side ensures the highest bid is always accessible at `begin()` in O(1). The sell side uses the default ascending order so the lowest ask is at `begin()`.

**Priority Queues for Time Priority**: Within a single price level, a `std::priority_queue<Order>` ordered by timestamp ensures the earliest order at that price is matched first, implementing fair FIFO execution within each price level.

**Resting Order Price Execution**: When two orders match, the trade executes at the price of the resting order (the one that was already in the book). This is the standard convention used by major exchanges to ensure price improvement for incoming orders.

---

## Build Instructions

### Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- CMake 3.16 or higher
- SQLite3 development libraries

### Build

```bash
mkdir -p build && cd build
cmake ..
make
```

CMake will automatically download and build Google Test v1.14.0 via FetchContent.

---

## Running the Application

From the `build/` directory:

```bash
./main.out
```

This runs a demonstration that creates buy and sell orders, adds them to the order book, and simulates the market. Executed trades are logged to `Database/TransactionHistory.db`.

---

## Running Tests

From the `build/` directory:

```bash
./all_tests
```

The test suite covers:

| Test Suite | Tests | Coverage |
|------------|-------|----------|
| **TestOrder** | Field access, timestamp accuracy, comparison operators | Order construction, data integrity, priority queue ordering |
| **TestOrderBook** | Add orders, duplicate price levels, buy/sell separation | Order insertion, map structure, equality comparison |
| **TestMatchingEngine** | Constructor, processOrder, simulateMarket (4 scenarios) | Partial fills, perfect matches, empty book, non-overlapping spreads, side validation |

---

## Roadmap

- [ ] Order cancellation and modification
- [ ] Multi-threaded order processing with concurrent read/write support
- [ ] Live market data integration via cryptocurrency exchange APIs (Ethereum, Bitcoin)
- [ ] REST or WebSocket API for external order submission
- [ ] Transaction logger test completion
- [ ] Market order support (execute immediately at best available price)
- [ ] Order book depth snapshots and L2 data output

---

## Technologies

| Technology | Purpose |
|------------|---------|
| **C++20** | Core language with modern features (structured bindings, constexpr, chrono) |
| **CMake** | Cross-platform build system |
| **SQLite3** | Lightweight embedded database for trade persistence |
| **Google Test** | Unit testing framework (v1.14.0, fetched at build time) |
