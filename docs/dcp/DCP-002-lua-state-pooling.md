# DCP-002: Lua State Pooling with Colored Pools

**Status:** Proposed  
**Author:** Design Review  
**Date:** 2026-02-18  
**Type:** Feature  

## Summary

Introduce an opt-in state pooling system with colored pools that allows reusing `lua_State` instances across multiple executions. Each pool "color" represents a specific state configuration (libraries, globals, hooks). The system reduces allocation overhead for high-performance scenarios while maintaining the simple "new state per call" default behavior for new developers.

## Motivation

### Current Behavior

Every execution creates a new `lua_State`:

| Method | State Lifecycle |
|--------|-----------------|
| `newState()` | Create → Use → Destroy |
| `newStateFor()` | Create → Use → Destroy |
| `Run()` | Create → Use → Destroy |
| `RunWithEnvironment()` | Create → Use → Destroy |

### Performance Impact

| Scenario | Current Overhead | Ideal |
|----------|------------------|-------|
| 100 sequential executions | 100 state creations | 1 state reused 100x |
| High-frequency scripting | Allocation per call | Pool amortization |
| Same configuration repeated | Repeated library loading | Load once, reuse |

### Goals

1. **Performance**: Reduce state creation overhead for repeated execution
2. **Simplicity**: Keep default behavior unchanged for new developers
3. **Flexibility**: Support multiple state configurations via "colored" pools
4. **Control**: Allow users to keep states outside pool for extended work

### Non-Goals

- Automatic state caching (explicit opt-in only)
- Thread-safe by default (opt-in feature)
- Pool monitoring/metrics

## Proposed Solution

### Core Components

#### 1. StatePool Class

Manages a collection of `lua_State` instances with identical configuration.

**Location:** `Source/Engine/StatePool.hpp`, `Source/Engine/StatePool.cpp`

```
StatePool {
    color: string
    config: PoolConfig
    available: queue<unique_ptr<LuaState>>
    currentSize: size_t
    
    acquire() -> unique_ptr<LuaState>
    release(unique_ptr<LuaState>)
    warmup(size_t n)
    drain()
    available() -> size_t
}
```

#### 2. PoolConfig Structure

Configuration for a specific pool color.

**Location:** `Source/Engine/PoolConfig.hpp`

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `libraries` | `vector<string>` | all | Standard libraries to load |
| `customLibraries` | `vector<shared_ptr<LuaLibrary>>` | empty | Custom C libraries |
| `globalVariables` | `LuaEnvironment` | empty | Base globals for all states |
| `hooks` | `vector<tuple<string, int, lua_Hook>>` | empty | Hooks to register |
| `maxSize` | `size_t` | 5 | Maximum states in pool |
| `exhaustionTimeoutMs` | `size_t` | 0 | Block timeout (0 = infinite) |

#### 3. PoolManager Class

Central manager for all colored pools.

**Location:** `Source/Engine/PoolManager.hpp`, `Source/Engine/PoolManager.cpp`

```
PoolManager {
    pools: map<string, unique_ptr<StatePool>>
    threadSafe: bool
    
    getPool(color) -> StatePool&
    createPool(color, config) -> StatePool&
    destroyPool(color)
    hasPool(color) -> bool
    listPools() -> vector<string>
    setThreadSafe(bool)
}
```

#### 4. PooledState Wrapper

RAII wrapper that automatically returns state to pool.

**Location:** `Source/Engine/PooledState.hpp`

```
PooledState {
    state: unique_ptr<LuaState>
    pool: StatePool*
    
    constructor(pool, state)
    destructor: returns state to pool
    operator*: access LuaState
    get(): LuaState*
}
```

### Predefined Pool Colors

| Color | Libraries | Use Case |
|-------|-----------|----------|
| `"default"` | all standard | General scripting |
| `"sandboxed"` | base, math, string, table | Untrusted code |
| `"minimal"` | base only | Maximum performance |
| `"io"` | base, io, os | File/system operations |

### API Extensions to LuaContext

#### Pool Management

```cpp
// Access pool manager
PoolManager& getPoolManager();

// Convenience methods
StatePool& getPool(const std::string& color = "default");
StatePool& createPool(const std::string& color, const PoolConfig& config);
bool hasPool(const std::string& color);
```

#### Pooled Execution

```cpp
// Execute using pooled state (auto-return)
void RunPooled(const std::string& name, const std::string& color = "default");

void RunWithEnvironmentPooled(
    const std::string& name, 
    const LuaEnvironment& env, 
    const std::string& color = "default"
);
```

#### Manual State Checkout

```cpp
// Checkout state for extended use (manual return required)
std::unique_ptr<Engine::LuaState> AcquirePooledState(
    const std::string& color = "default"
);

// Return state to pool
void ReleasePooledState(
    std::unique_ptr<Engine::LuaState> state,
    const std::string& color = "default"
);

// RAII-style checkout
PooledState AcquirePooledStateRAII(
    const std::string& color = "default"
);
```

### Usage Examples

#### Example 1: Simple Pooled Execution

```cpp
LuaContext ctx;

// One-time setup
ctx.CompileString("myscript", "print('Hello')");

// Execute using pooled state (auto-acquire, auto-release)
ctx.RunPooled("myscript");
ctx.RunPooled("myscript");
ctx.RunPooled("myscript");
// Only 1 state created and reused
```

#### Example 2: Using Different Colors

```cpp
LuaContext ctx;

ctx.CompileString("safe_script", "result = 2 + 2");
ctx.CompileString("file_script", "f = io.open('test.txt')");

// Use sandboxed pool for untrusted code
ctx.RunPooled("safe_script", "sandboxed");

// Use io pool for file operations
ctx.RunPooled("file_script", "io");
```

#### Example 3: Manual State Checkout

```cpp
LuaContext ctx;
ctx.CompileString("init", "counter = 0");
ctx.CompileString("increment", "counter = counter + 1");
ctx.CompileString("result", "return counter");

// Checkout state for extended work
auto state = ctx.AcquirePooledState("default");

// Execute multiple scripts on same state
ctx.getRegistry().getByName("init")->UploadCode(*state);
lua_pcall(*state, 0, 0, 0);

for (int i = 0; i < 100; i++) {
    ctx.getRegistry().getByName("increment")->UploadCode(*state);
    lua_pcall(*state, 0, 0, 0);
}

// Counter persists across calls
ctx.getRegistry().getByName("result")->UploadCode(*state);
lua_pcall(*state, 0, 1, 0);
int counter = lua_tointeger(*state, -1);
// counter == 100

// Return to pool when done
ctx.ReleasePooledState(std::move(state), "default");
```

#### Example 4: RAII-Style State Management

```cpp
LuaContext ctx;

{
    auto pooled = ctx.AcquirePooledStateRAII("default");
    
    // Use *pooled or pooled.get()
    lua_pushstring(*pooled, "test");
    
    // State automatically returns to pool at end of scope
}
```

#### Example 5: Custom Pool Configuration

```cpp
LuaContext ctx;

PoolConfig config;
config.libraries = {"base", "math"};
config.globalVariables["version"] = std::make_shared<LuaTString>("1.0");
config.maxSize = 3;
config.exhaustionTimeoutMs = 5000; // 5 second timeout

ctx.createPool("math_only", config);

ctx.CompileString("calc", "return math.sqrt(16)");
ctx.RunPooled("calc", "math_only");
```

#### Example 6: Pool Warmup

```cpp
LuaContext ctx;

// Pre-create all states in pool
auto& pool = ctx.getPool("default");
pool.warmup(5); // Create 5 states upfront

// Now all AcquirePooledState calls are allocation-free
```

### Pool Behavior

#### Acquisition Logic

```
acquire():
    if available queue not empty:
        return queue.pop_front()
    
    if currentSize < maxSize:
        state = createNewState(config)
        currentSize++
        return state
    
    // Pool exhausted
    if exhaustionTimeoutMs == 0:
        block until state released
    else if exhaustionTimeoutMs > 0:
        block with timeout
        if timeout:
            throw PoolExhaustedException
    else:
        throw PoolExhaustedException
```

#### Release Logic

```
release(state):
    if currentSize > maxSize:
        // Pool was reduced in config, destroy excess
        destroy(state)
        currentSize--
    else:
        reset(state)
        available.push_back(state)
```

#### State Reset

When a state is released, it is reset before being made available:

| Action | Lua Call | Purpose |
|--------|----------|---------|
| Clear stack | `lua_settop(L, 0)` | Remove leftover values |
| Reset hooks | `lua_sethook(L, NULL, 0, 0)` | Clear debug hooks |
| Restore globals | iterate config | Reset to pool defaults |

### Thread Safety

Thread safety is opt-in via `PoolManager::setThreadSafe(true)`.

| Mode | Behavior | Overhead |
|------|----------|----------|
| Off (default) | No synchronization | None |
| On | Mutex on acquire/release | Minimal |

When enabled:
- Each `StatePool` has its own mutex
- `PoolManager` has mutex for pool creation/destruction
- Individual `lua_State` instances are NOT made thread-safe

### Default Configuration

| Setting | Value |
|---------|-------|
| Pool size | 5 states |
| Exhaustion timeout | Infinite (block) |
| Thread safety | Disabled |
| Validation on release | None |
| Reset timing | On release |

### File Structure

```
Source/
├── Engine/
│   ├── StatePool.hpp
│   ├── StatePool.cpp
│   ├── PoolConfig.hpp
│   ├── PoolManager.hpp
│   ├── PoolManager.cpp
│   └── PooledState.hpp
├── LuaContext.hpp  (extended)
├── LuaContext.cpp  (extended)
└── UnitTest/
    ├── TestStatePool.cpp
    ├── TestPoolManager.cpp
    └── TestLuaContextPooling.cpp
```

## Impact Analysis

### API Compatibility

- **Breaking Changes:** None
- **New API:** Additive only
- **Default Behavior:** Unchanged

### Performance Expectations

| Scenario | Current | With Pooling | Improvement |
|----------|---------|--------------|-------------|
| Single execution | ~1ms | ~1ms | None |
| 100 sequential (same config) | ~100ms | ~15-25ms | 4-6x |
| 100 sequential (warmed pool) | ~100ms | ~10-15ms | 7-10x |
| State creation amortized | Per call | Once per pool | Significant |

### Memory Impact

| Scenario | Memory Usage |
|----------|--------------|
| No pooling (current) | 0 (transient) |
| Default pool (5 states) | ~5-10MB |
| All 4 predefined pools | ~20-40MB |

Users control memory via `maxSize` configuration.

### Dependencies

- No new external dependencies
- Uses existing C++17 features (std::mutex for thread-safe mode)

## Implementation Checklist

### Phase 1: Core Pool Infrastructure

- [ ] Create `PoolConfig.hpp` with configuration structure
- [ ] Create `StatePool.hpp` and `StatePool.cpp` with basic acquire/release
- [ ] Add state reset functionality
- [ ] Add pool warmup and drain methods
- [ ] Unit tests for `StatePool`

### Phase 2: Pool Manager

- [ ] Create `PoolManager.hpp` and `PoolManager.cpp`
- [ ] Implement pool creation and retrieval
- [ ] Define predefined colors and configurations
- [ ] Unit tests for `PoolManager`

### Phase 3: LuaContext Integration

- [ ] Add `PoolManager` member to `LuaContext`
- [ ] Add `RunPooled()` and `RunWithEnvironmentPooled()` methods
- [ ] Add `AcquirePooledState()` and `ReleasePooledState()` methods
- [ ] Integration tests

### Phase 4: RAII Wrapper

- [ ] Create `PooledState.hpp` with RAII wrapper
- [ ] Add `AcquirePooledStateRAII()` method
- [ ] Unit tests for `PooledState`

### Phase 5: Thread Safety

- [ ] Add mutex members to `StatePool` and `PoolManager`
- [ ] Implement `setThreadSafe()` configuration
- [ ] Thread safety tests

### Phase 6: Documentation and Examples

- [ ] Update Doxygen comments
- [ ] Add pooling example to `Source/Example/`
- [ ] Update user documentation

## Open Items

| Item | Resolution |
|------|------------|
| Exhaustion timeout default | Infinite (block) - configurable via `exhaustionTimeoutMs` |
| Pool priority/fallback | Not implemented (user manages multiple pools) |
| Metrics/monitoring | Not in scope |

## References

- Lua 5.4 Reference Manual: `lua_State` lifecycle
- Existing `LuaContext::newState()` implementation
- DCP-001: Precedent for design change proposals
