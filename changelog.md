## Completed List



## In Progress List
Implement tests for Order, OrderBook, and Matching Engine



## Changes List

### Phase 1: Core Architecture Fixes (Days 1-3)
These aren't features — they're the baseline that any strong C++ engineer would expect to see.

#### 1. Dependency Injection for TransactionLogger - DONE
- Define an `ITransactionLogger` abstract interface with a pure virtual `logTrade()` method
- Have `TransactionLogger` inherit from `ITransactionLogger`
- Change `MatchingEngine` to accept an `ITransactionLogger&` or `std::unique_ptr<ITransactionLogger>` instead of constructing one internally
- Create a `MockTransactionLogger` that implements the interface and records calls in a `std::vector<TradeRecord>` for test assertions
- This eliminates the SQLite dependency from all unit tests and lets the test suite run without a database
- Refactor all existing `MatchingEngine` tests to use the mock

#### 2. Encapsulate OrderBook Internals - DONE
- Make `buyOffers` and `sellOffers` private
- Expose a clean public interface: `addOrder()`, `getBestBid()`, `getBestAsk()`, `removeBestBid()`, `removeBestAsk()`, `isBuySideEmpty()`, `isSellSideEmpty()`, `getBuyDepth()`, `getSellDepth()`
- Refactor `MatchingEngine::simulateMarket()` to use these methods instead of directly manipulating the maps
- Update all tests to use the new interface
- This is the single biggest code quality signal — it shows you understand encapsulation and API design

#### 3. Fix Priority Queue Ordering for Time Priority - DONE
- `std::priority_queue` is a max-heap, so `operator<` putting earlier timestamps as "less" means the *latest* order sits on top — the opposite of FIFO
- Either flip `operator<` to return `timestamp > b.timestamp`, or provide a custom comparator to the priority queue
- Add a targeted unit test: create 3 orders at the same price with deliberate sleep gaps, add them to a price level, and assert that `top()` returns the earliest order
- Document the fix in a commit message that explains the bug and the reasoning — this shows you understand data structure invariants

#### 4. General Code Cleanup - DONE
- Remove all commented-out debug code from `Main.cpp`, test files, and `MatchingEngine.cpp`
- Remove unnecessary includes: `<sqlite3.h>` from `OrderBook.h`, unused headers from source files
- Add `const` to `getBook()` and `getLog()` method signatures
- Rename constructor parameters from `priceTemp`/`quantityTemp` to `rawPrice`/`rawQuantity`
- Simplify brace initialization: `Order order1(type, 20.0, 1.0, 101, 111)` instead of `Order order1 {Order(type, 20.0, 1.0, 101, 111)}`



### Phase 2: Performance Benchmarking (Days 4-6)
This is the highest-signal addition for quant recruiting. Every quant firm cares about latency.

#### 5. Microbenchmark the Matching Engine
- Create a `benchmarks/` directory with a dedicated benchmark binary
- Use `std::chrono::high_resolution_clock` to measure:
  - Order insertion latency (time to call `addOrder()` for a single order)
  - Single match latency (time for one `processOrder()` call)
  - Full market simulation throughput (orders matched per second with N=1000, 10000, 100000 orders)
- Generate randomized orders with realistic price distributions (normal distribution around a center price using `<random>`)
- Print results in a clean table: operation, N, mean latency, p50, p99, throughput
- Run benchmarks with compiler optimizations on (`-O2`)

#### 6. Profile, Identify a Bottleneck, and Optimize
- Profile with `perf` or `valgrind --tool=callgrind` to find where time is spent
- Likely bottleneck candidates: `std::map` node allocation overhead, priority queue copy overhead, or the `operator==` on OrderBook draining full queues
- Investigate whether replacing `std::map` with a flat sorted container or using `std::unordered_map` with a separate best-price tracker improves insertion latency
- Investigate whether adding move semantics to `Order` reduces matching overhead
- Document the results: write a `PERFORMANCE.md` with before/after measurements, what you tried, and why you chose the final approach
- Even if the optimization is modest, the *process* of measure → hypothesize → test → document is exactly what quant firms want to see



### Phase 3: Quantitative Features (Days 7-10)
This adds the "quant" to your project and gives you something mathematical to discuss in interviews.

#### 7. Real-Time Market Statistics Engine
- Create a `MarketStats` class that computes rolling statistics from executed trades:
  - **VWAP** (Volume-Weighted Average Price): `sum(price * quantity) / sum(quantity)` over a configurable window
  - **Bid-ask spread**: difference between best bid and best ask, tracked over time
  - **Trade volume**: rolling sum of quantity traded in the last N seconds
  - **Simple volatility**: standard deviation of trade prices over the last N trades
- Use efficient online algorithms — Welford's algorithm for running variance, not naive recompute-from-scratch
- Store statistics in a ring buffer or sliding window structure to avoid unbounded memory growth
- Print a market summary after each `simulateMarket()` call showing current VWAP, spread, volume, and volatility
- Add unit tests that verify statistical calculations against hand-computed expected values

#### 8. Live Price Feed Integration
- Use `cpp-httplib` (header-only, no build complexity) to make HTTP GET requests to the CoinGecko free API (`/api/v3/simple/price`)
- Fetch real BTC/ETH/SOL prices on startup
- Create an `OrderGenerator` class that produces randomized orders around the real price:
  - Buy orders: price drawn from `N(market_price * 0.999, sigma)` (slightly below market)
  - Sell orders: price drawn from `N(market_price * 1.001, sigma)` (slightly above market)
  - Quantities drawn from a log-normal distribution to simulate realistic order sizes
  - Use `<random>` with `std::mt19937` and proper seeding
- Generate batches of 100-1000 orders and feed them through the matching engine
- This replaces the hardcoded orders in `Main.cpp` and makes the demo actually compelling



### Phase 4: Concurrency (Days 11-16)
This is the feature that makes the project memorable. Correct concurrent code is hard and recruiters know it.

#### 9. Thread-Safe Order Book
- Add a `std::shared_mutex` to `OrderBook` for reader-writer locking
- Read operations (`getBestBid()`, `getBestAsk()`, depth queries) take a shared lock
- Write operations (`addOrder()`, `removeBestBid()`, `removeBestAsk()`) take an exclusive lock
- Alternatively, use a `std::mutex` with `std::lock_guard` if reader-writer distinction isn't needed yet — correctness first, optimization second
- Add stress tests: multiple threads calling `addOrder()` concurrently, verify no data corruption

#### 10. Producer-Consumer Architecture
- Producer thread: runs the `OrderGenerator` from Phase 3, generates orders at a configurable rate (e.g., 100 orders/second), pushes them into a thread-safe queue
- Consumer thread: pulls orders from the queue, adds them to the order book, and runs the matching engine
- Use `std::condition_variable` to avoid busy-waiting — producer notifies consumer when orders are available
- Add a `std::atomic<bool>` shutdown flag for clean termination
- Create a simple command-line interface: start/stop the market, print statistics, adjust order generation rate
- Log throughput: "Processed N orders in M seconds, average latency X microseconds"

#### 11. Document the Concurrency Model
- Add a section to `README.md` explaining the threading model with a diagram
- Describe what's locked, when, and why
- Mention any race conditions you encountered and how you fixed them
- If you tried lock-free approaches and they didn't work out, say so — that's a great interview talking point



### Phase 5: Polish (Days 17-19)

#### 12. Order Cancellation
- Add a `std::unordered_map<int, OrderStatus>` mapping `transactionId` to status (active, cancelled, filled)
- Add a `cancelOrder(int transactionId)` method to `OrderBook` that marks the order as cancelled
- Modify `simulateMarket()` to skip cancelled orders when they reach the top of the queue (lazy deletion)
- Add tests: cancel an order, verify it doesn't match; cancel a non-existent order, verify error handling

#### 13. README Rewrite
- Add a Performance section with benchmark results and the optimization story from Phase 2
- Add a Concurrency section with the threading architecture diagram from Phase 4
- Add a Market Statistics section explaining VWAP, spread, and volatility calculations
- Include sample output showing the system running with live prices, concurrent order generation, and real-time statistics
- Remove the current demo code walkthrough and replace it with something that reflects the actual system

#### 14. Clean Git History
- Each phase should be its own set of well-structured commits with clear messages
- Squash any "fix typo" or "oops" commits before making the repo public
- Tag releases: `v0.1` (current state), `v0.2` (architecture fixes), `v0.3` (benchmarking), `v0.4` (quant features), `v1.0` (concurrency + polish)
- A recruiter will look at your commit history — make it tell a story of deliberate, incremental improvement