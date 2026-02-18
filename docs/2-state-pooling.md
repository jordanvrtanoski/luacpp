# State Pooling

## Overview

State pooling is a performance optimization feature that allows reusing Lua state instances across multiple script executions. Instead of creating a new `lua_State` for each execution (which involves memory allocation and library loading), states are borrowed from a pool and returned after use.

### When to Use Pooling

| Scenario | Recommendation |
|----------|----------------|
| High-frequency script execution | Use pooling |
| Same script executed repeatedly | Use pooling |
| One-off script execution | Regular `Run()` is fine |
| Scripts with different library requirements | Use colored pools |
| Multi-threaded environment | Use pooling with thread safety enabled |

### Performance Benefits

| Operation | Without Pooling | With Pooling |
|-----------|-----------------|--------------|
| State creation | Per execution | Once, then reused |
| Library loading | Per execution | Once per state |
| Memory allocation | Per execution | Amortized |
| 100 sequential runs | ~100 allocations | 1-5 allocations |

---

## Quick Start

The simplest way to use state pooling is with `RunPooled()`:

```cpp
#include <LuaCpp.hpp>

using namespace LuaCpp;

int main() {
    LuaContext ctx;
    
    // Compile a script
    ctx.CompileString("hello", "print('Hello from pooled state!')");
    
    // Execute using a pooled state
    ctx.RunPooled("hello");
    
    // The state is automatically returned to the pool
    return 0;
}
```

---

## Pool Colors

Pool colors identify different pool configurations. Each color represents a pool with specific libraries, global variables, and hooks pre-configured.

### Predefined Pools

| Color | Libraries | Use Case |
|-------|-----------|----------|
| `"default"` | All standard libraries | General scripting |
| `"sandboxed"` | base, math, string, table | Safe execution of untrusted code |
| `"minimal"` | base only | Maximum performance, minimal features |
| `"io"` | base, io, os | File and system operations |

### Using Different Pool Colors

```cpp
LuaContext ctx;

ctx.CompileString("calc", "result = math.sqrt(144)");
ctx.CompileString("file_op", "f = io.open('data.txt', 'r')");

// Execute on sandboxed pool (has math, no io)
ctx.RunPooled("calc", "sandboxed");

// Execute on io pool (has io and os)
ctx.RunPooled("file_op", "io");
```

---

## Custom Pools

Create custom pools with specific configurations using `PoolConfig`.

### Pool Configuration Options

| Option | Type | Description |
|--------|------|-------------|
| `libraries` | `vector<string>` | Lua standard libraries to load |
| `globalVariables` | `PoolEnvironment` | Pre-defined global variables |
| `hooks` | `vector<tuple>` | Debug hooks to register |
| `maxSize` | `size_t` | Maximum states in pool (default: 5) |

### Available Libraries

| Library Name | Description |
|--------------|-------------|
| `"base"` | Basic functions (print, tonumber, etc.) |
| `"coroutine"` | Coroutine support |
| `"debug"` | Debug library |
| `"io"` | Input/output |
| `"math"` | Mathematical functions |
| `"os"` | Operating system functions |
| `"package"` | Module loading |
| `"string"` | String manipulation |
| `"table"` | Table manipulation |
| `"utf8"` | UTF-8 support |

### Creating a Custom Pool

```cpp
LuaContext ctx;

PoolConfig config;
config.libraries = {"base", "math"};
config.maxSize = 3;
config.AddGlobalVariable("app_version", std::make_shared<LuaTString>("1.0.0"));

StatePool& pool = ctx.createPool("math_only", config);

ctx.CompileString("calc", "print(math.sqrt(100) .. ' (v' .. app_version .. ')')");
ctx.RunPooled("calc", "math_only");
```

### Fluent Configuration

`PoolConfig` supports method chaining:

```cpp
PoolConfig config;
config.SetLibraries({"base", "math"})
      .SetMaxSize(3)
      .AddGlobalVariable("debug", std::make_shared<LuaTBoolean>(true));
```

---

## Manual State Management

For scenarios where you need to execute multiple scripts on the same state, use manual acquire/release.

### Acquire and Release

```cpp
LuaContext ctx;
ctx.CompileString("init", "counter = 0");
ctx.CompileString("increment", "counter = counter + 1");
ctx.CompileString("get", "print(counter)");

// Acquire state from pool
auto state = ctx.AcquirePooledState("default");

// Execute multiple scripts on the same state
ctx.RunWithEnvironmentPooled("init", LuaEnvironment(), "default");
ctx.RunWithEnvironmentPooled("increment", LuaEnvironment(), "default");
ctx.RunWithEnvironmentPooled("increment", LuaEnvironment(), "default");
ctx.RunWithEnvironmentPooled("get", LuaEnvironment(), "default");

// Return state to pool
ctx.ReleasePooledState(std::move(state), "default");
```

### RAII Wrapper

For exception-safe state management, use `PooledState`:

```cpp
LuaContext ctx;
ctx.CompileString("work", "print('working...')");

{
    // State automatically returns to pool at scope exit
    auto pooled = ctx.AcquirePooledStateRAII("default");
    
    ctx.RunPooled("work");
    
    // Even if an exception is thrown here,
    // the state will be returned to the pool
}
```

---

## Thread Safety

State pooling can be made thread-safe for use in multi-threaded applications.

### Enabling Thread Safety

```cpp
LuaContext ctx;

// Get the pool and enable thread safety
StatePool& pool = ctx.getPool("default");
pool.setThreadSafe(true);

// Or enable for all pools
ctx.getPoolManager().setThreadSafe(true);
```

### Multi-threaded Example

```cpp
#include <thread>
#include <vector>

LuaContext ctx;
ctx.CompileString("work", "print('Thread working...')");

// Create and warmup a thread-safe pool
StatePool& pool = ctx.createPool("threads", PoolConfig().SetMaxSize(4));
pool.setThreadSafe(true);
pool.warmup(4);

// Spawn multiple threads
std::vector<std::thread> threads;
for (int i = 0; i < 4; i++) {
    threads.emplace_back([&ctx]() {
        try {
            ctx.RunPooled("work", "threads");
        } catch (const PoolExhaustedException& e) {
            // Handle pool exhaustion
        }
    });
}

for (auto& t : threads) {
    t.join();
}
```

**Note:** While the pool is thread-safe, individual `lua_State` instances are NOT. Never share a single acquired state between threads.

---

## Pool Statistics

Monitor pool health and usage with statistics methods:

```cpp
StatePool& pool = ctx.getPool("default");

std::cout << "Color: " << pool.getColor() << "\n";
std::cout << "Max size: " << pool.getMaxSize() << "\n";
std::cout << "Current size: " << pool.getCurrentSize() << "\n";
std::cout << "Available: " << pool.availableCount() << "\n";
std::cout << "Checked out: " << pool.checkedOutCount() << "\n";
std::cout << "Thread safe: " << (pool.isThreadSafe() ? "yes" : "no") << "\n";
```

---

## Pool Management

### Warmup

Pre-create states to eliminate allocation overhead during execution:

```cpp
StatePool& pool = ctx.getPool("default");
pool.warmup(5);  // Create 5 states upfront

// Now all AcquirePooledState() calls will be allocation-free
```

### Drain

Remove all available states from the pool:

```cpp
StatePool& pool = ctx.getPool("default");
pool.drain();  // Remove all available states

// Pool is now empty, next acquire will create a new state
```

### Listing Pools

```cpp
PoolManager& manager = ctx.getPoolManager();

for (const auto& color : manager.listPools()) {
    std::cout << "Pool: " << color << "\n";
}
```

### Destroying Custom Pools

```cpp
ctx.getPoolManager().destroyPool("custom_pool");
```

**Note:** Predefined pools (`default`, `sandboxed`, `minimal`, `io`) cannot be destroyed.

---

## Error Handling

### PoolExhaustedException

Thrown when acquiring a state from an exhausted pool:

```cpp
try {
    auto state = ctx.AcquirePooledState("tiny_pool");
} catch (const PoolExhaustedException& e) {
    std::cout << "Pool exhausted: " << e.what() << "\n";
}
```

### Pool Not Found

Thrown when accessing a non-existent pool:

```cpp
try {
    StatePool& pool = ctx.getPoolManager().getPool("nonexistent");
} catch (const std::runtime_error& e) {
    std::cout << "Pool not found: " << e.what() << "\n";
}
```

### Duplicate Pool

Thrown when creating a pool that already exists:

```cpp
try {
    ctx.createPool("default", PoolConfig());  // "default" already exists
} catch (const std::runtime_error& e) {
    std::cout << "Pool already exists: " << e.what() << "\n";
}
```

---

## API Reference

### LuaContext Pooling Methods

| Method | Description |
|--------|-------------|
| `getPoolManager()` | Get the pool manager |
| `getPool(color)` | Get a pool by color name |
| `hasPool(color)` | Check if a pool exists |
| `createPool(color, config)` | Create a custom pool |
| `RunPooled(name, color)` | Execute using pooled state |
| `RunWithEnvironmentPooled(name, env, color)` | Execute with environment using pooled state |
| `AcquirePooledState(color)` | Acquire state for manual use |
| `ReleasePooledState(state, color)` | Return state to pool |
| `AcquirePooledStateRAII(color)` | Acquire state with RAII wrapper |

### StatePool Methods

| Method | Description |
|--------|-------------|
| `acquire()` | Get a state from the pool |
| `release(state)` | Return a state to the pool |
| `warmup(n)` | Pre-create n states |
| `drain()` | Remove all available states |
| `getColor()` | Get the pool color name |
| `getConfig()` | Get the pool configuration |
| `getMaxSize()` | Get maximum pool size |
| `getCurrentSize()` | Get current number of states |
| `availableCount()` | Get number of available states |
| `checkedOutCount()` | Get number of checked-out states |
| `setThreadSafe(bool)` | Enable/disable thread safety |
| `isThreadSafe()` | Check if thread safety is enabled |

### PoolManager Methods

| Method | Description |
|--------|-------------|
| `getPool(color)` | Get a pool by color |
| `createPool(color, config)` | Create a custom pool |
| `destroyPool(color)` | Destroy a custom pool |
| `hasPool(color)` | Check if a pool exists |
| `listPools()` | List all pool colors |
| `setThreadSafe(bool)` | Enable thread safety for all pools |
| `isThreadSafe()` | Check thread safety status |

---

## Examples

See the example files for complete demonstrations:

- `example_StatePool.cpp` - Basic pooling usage
- `example_StatePoolAdvanced.cpp` - Advanced patterns including thread safety
