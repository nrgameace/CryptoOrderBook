# Concurrency

```
  Main Thread
  │
  ├─ spawn ──────────────────────────────────────────────────────────-|
  │                                                                   │
  ▼                                                                   ▼
┌─────────────────────────────────┐               ┌──────────────────────────────────────┐
│       producerThread (jthread)  │               │       consumerThread (jthread)       │
│─────────────────────────────────│               │──────────────────────────────────────│
│ check stop_token                │               │ wait on condition_variable_any       │
│ generate orders (PriceGenerator)│               │  (wakes on notify or stop_requested) │
│   [no lock held]                │               │                                      │
│                                 │               │ acquire mtx                          │
│ acquire mtx ◄───── contention ──┼───────────────► drain transitionQueue → local batch  │
│ push batch → transitionQueue    │               │ release mtx                          │
│ release mtx                     │               │   [no lock held]                     │
│ notify_one()  ──────────────────┼──────────────►│                                      │
│                                 │               │ MatchingEngine::processOrder()       │
│ sleep 100ms                     │               │   └─ OrderBook::addOrder()           │
│ loop / exit on stop_requested   │               │        └─ unique_lock<shared_mutex>  │
└─────────────────────────────────┘               │                                      │
                                                  │ MatchingEngine::simulateMarket()     │
                                                  │   ├─ getBestBid/Ask()                │
                                                  │   │    └─ shared_lock<shared_mutex>  │
                                                  │   └─ removeBestBid/Ask()             │
                                                  │        └─ unique_lock<shared_mutex>  │
                                                  │                                      │
                                                  │ loop / exit on stop_requested        │
                                                  └──────────────────────────────────────┘

  Shared State
  ┌──────────────────────────────────────────────────────────────────────┐
  │  transitionQueue (std::queue<Order>)  ── guarded by std::mutex       │
  │  OrderBook::buyOffers / sellOffers    ── guarded by std::shared_mutex│
  │    shared_lock  → getBestBid, getBestAsk, isBuySideEmpty, depths     │
  │    unique_lock  → addOrder, removeBestBid, removeBestAsk             │
  └──────────────────────────────────────────────────────────────────────┘

  Shutdown
  ┌──────────────────────────┐
  │  stopper (std::thread)   │
  │  blocks on std::cin.get()│
  │  → simulator.stop()      │
  │  → jthread::request_stop │
  │  → both loops exit       │
  └──────────────────────────┘
```

## Overall Strategy

The concurrency logic lives in `Simulator.cpp`. The simulator uses two `std::jthread`s: one running a producer loop that generates incoming orders, and one running a consumer loop that acts as the market, matching orders against the order book.

### Producer Loop

The producer loop first checks that a stop has not been requested via its `std::stop_token`. It then generates a batch of orders using the `PriceGenerator` class and stores them in a local vector. Only after generation is complete does the producer acquire the queue mutex, transfer the batch into the shared `transitionQueue`, release the lock, and notify the consumer. Holding the mutex only during the transfer — not during generation — keeps the critical section minimal.

### Consumer Loop

The consumer loop waits on a `std::condition_variable_any`, which wakes either when the producer notifies that new orders are available or when a stop is requested. Once awake, the consumer drains every order from the `transitionQueue` into a local vector, releases the queue mutex, and only then passes the orders to the `MatchingEngine`. The `OrderBook` has its own internal locking, so the consumer does not need to hold any external lock when calling `addOrder()` or `simulateMarket()`.

### Transition Queue

The `transitionQueue` is guarded by a `std::mutex` that must be held for all reads and writes. Both the producer and consumer acquire this mutex only long enough to move orders in or out. The actual work of generating orders and matching them happens outside the critical section. This keeps contention on the queue low and prevents the producer or consumer from blocking each other during their expensive operations.

### OrderBook Mutex

The `OrderBook` is guarded by its own shared mutex unique to that object's instance. This lock is a `shared_mutex` which allows any thread from the `Simulator` class to call the method and activate the lock preventing the other from accessing it. This design ultimately allows for an easier approach of preventing double-accessing of the orderbook while allowing for future improvements down the line. One important dis-advantage to this strategy is that if a class were to first acquire the lock for the orderbook, then the lock for the transition queue, this could create a deadlock between the two. As a result, all concurrent tasks must be run from the `Simulator` class as it prevents this deadlock condition and is more logical in the scope of the entire order book.