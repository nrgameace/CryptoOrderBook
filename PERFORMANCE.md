# Performance

This document records benchmark results, profiling findings, and optimization decisions for the CryptoOrderBook matching engine.

---

## Benchmark Environment

| Property | Value |
|----------|-------|
| CPU | AMD Ryzen 7 3700x|
| OS | [e.g. Ubuntu 22.04] |
| Compiler | [e.g. GCC 13.1] |
| Build flags | `-O2 -std=c++20` |
| Commit | [git hash] |

All benchmarks were run with the `benchmarks` binary built in Release mode (`cmake -DCMAKE_BUILD_TYPE=Release ..`). Each result is the median of [N] runs to reduce noise from OS scheduling.

---

## Results

### `addOrder` — Order Book Insertion Latency

Measures the time to call `OrderBook::addOrder()` for a single buy order with a randomly drawn price in [90, 110]. The RNG is seeded and advanced outside the timed region.

| N | Total (ns) | Mean (ns/op) | p50 (ns) | p99 (ns) |
|---|-----------|-------------|---------|---------|
| 1 | 13636 | 13636 | 13636 | 13636 |
| 1,000 | 586045 | 586 | 431 | 3888 |
| 10,000 | 3271782 | 327 | 271 | 1954 |
| 100,000 | 38840062 | 388 | 301 | 611 |

**Observations:**

The N=1 result (13,636 ns) is a cold-start artifact and not representative of steady-state performance; by N=1,000 the allocator and cache are warm and insertion settles into a consistent 300-580 ns/op range. The mean decreasing from N=1k to N=10k is expected given the [90, 110] price range produces only 21 distinct map buckets: once all price levels exist, each call reduces to a heap push into an already-allocated node, flattening the per-call cost. At N=100k, slight mean growth reflects memory pressure as large priority queues begin causing cache misses, though p99 actually improves at that scale because the backing vectors have stopped resizing. Note that the narrow price range makes these numbers artificially favorable; a production order book with thousands of distinct price levels would see O(log n) map traversal become the dominant cost rather than the heap push.
---

### `executeTrade` — Single Match Latency

Measures the time for one `MatchingEngine::processOrder()` call against pre-generated buy/sell order pairs. Orders are not inserted into the book first, so this isolates the trade execution path (price check, quantity arithmetic, `logTrade`) from map and priority queue overhead.

| N | Total (ns) | Mean (ns/op) | p50 (ns) | p99 (ns) |
|---|-----------|-------------|---------|---------|
| 1 | 60 | 60 | 60 | 60 |
| 1,000 | 56241 | 56 | 50 | 320 |
| 10,000 | 582443 | 58 | 50 | 70 |
| 100,000 | 8156783 | 81 | 50 | 100 |

**Observations:**

`processOrder` is significantly faster than `addOrder` at every scale (50-81 ns/op vs 300-580 ns/op), which makes sense given the execution path is pure arithmetic: a price comparison, some integer subtraction, and a `logTrade` call with no heap allocation or tree traversal involved. The p99 spike at N=1,000 (320 ns) is the same backing-vector reallocation story as `addOrder`, collapsing to 70-100 ns by N=10k-100k once the mock logger's internal vector has stabilized. The slight mean increase at N=100k is likely the mock logger's `std::vector<TradeRecord>` growing large enough to cause occasional cache misses on push. In a production configuration using the real `TransactionLogger`, these numbers would be meaningfully higher given each call would involve a SQLite prepared statement execution rather than a vector push.
---

### `simulateMarket` — Full Market Throughput

Measures the total time and throughput (orders/sec) for `MatchingEngine::simulateMarket()` over a pre-loaded book of N buy and N sell orders. Because the book state cannot be reset between iterations, this is reported as a single-run throughput figure rather than a per-iteration mean.

| N | Total (ns) | Throughput (orders/sec) |
|---|-----------|------------------------|
| 1 | 1613 | 619,963 |
| 1,000 | 464186 | 2,154,310 |
| 10,000 | 6307111 | 1,585,510 |
| 100,000 | 71803386 | 1,392,690 |

**Observations:**

Throughput peaks at N=1,000 (2.15M orders/sec) rather than N=1, which reflects the same cache warming pattern seen in `addOrder`: the first run pays cold-start costs that are amortized away once the allocator and map nodes are warm. The gradual decline from N=1,000 to N=100,000 (2.15M to 1.39M orders/sec) is expected given `simulateMarket` calls `getBestBid`, `getBestAsk`, `removeBestBid`, and `removeBestAsk` in a tight loop, each acquiring a lock and touching the map, so as the priority queues grow deeper the cache miss pressure from `addOrder` compounds across every iteration. The throughput staying in the 1.4-2.2M orders/sec range across three orders of magnitude of input is actually a strong result, suggesting the price-time priority matching loop scales gracefully. Note that these numbers reflect a pre-loaded book with a narrow [90, 110] price range and uniform order sizes, so real-world throughput would be lower given partial fills, wider spreads, and the overhead of the live producer-consumer queue.

---

## Profiling

### Tool

[e.g. `perf stat`, `valgrind --tool=callgrind`, `gprof`]

### Hotspots

[Paste or summarize the top entries from your profiler output here. Example format:]

```
[function name]         [% time]    [description of what it's doing]
```

### Analysis

[Describe what the profiler told you. Which functions dominated? Was the bottleneck where you expected it to be? Common candidates in this architecture:]

- `std::map` node allocation and tree rebalancing on `addOrder`
- `std::priority_queue` copy overhead during push/pop
- Lock contention on `std::shared_mutex` under concurrent access
- `MockTransactionLogger::logTrade` vector push overhead

---

## Optimization Attempts

### Attempt 1: [Name of what you tried]

**Hypothesis:** [What did you think would be slow and why?]

**Change:** [What did you actually modify?]

**Result:**

| Benchmark | Before | After | Delta |
|-----------|--------|-------|-------|
| addOrder p50 | | | |
| addOrder p99 | | | |
| simulateMarket throughput | | | |

**Conclusion:** [Did it help? Why or why not? Did it introduce any tradeoffs — complexity, correctness risk, cache behavior?]

---

### Attempt 2: [Name of what you tried]

**Hypothesis:**

**Change:**

**Result:**

| Benchmark | Before | After | Delta |
|-----------|--------|-------|-------|
| addOrder p50 | | | |
| addOrder p99 | | | |
| simulateMarket throughput | | | |

**Conclusion:**

---

## Final State

[Summarize what the final optimized version looks like and what you decided to keep. If you reverted an optimization, explain why — correctness, maintainability, and marginal gains are all valid reasons.]

### Key Takeaways

- [One sentence on the biggest bottleneck you found]
- [One sentence on the most effective change you made]
- [One sentence on something you tried that didn't work and why]

---

## Future Work

- [ ] Profile under concurrent load (multiple producer threads) — the `std::shared_mutex` contention profile will look very different under real concurrency than in the single-threaded benchmark
- [ ] Investigate replacing `std::map` with a flat sorted container (`std::vector` + binary search) for small books where cache locality may outweigh tree traversal cost
- [ ] Investigate lock-free queue for `transitionQueue` to reduce producer-consumer handoff latency
- [ ] Measure real `TransactionLogger` (SQLite) overhead vs `MockTransactionLogger` to quantify the database write cost per trade
