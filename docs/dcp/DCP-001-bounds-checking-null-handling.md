# DCP-001: Bounds Checking and Null Pointer Handling

**Status:** Implemented  
**Author:** Code Review  
**Date:** 2026-02-17  
**Type:** Bug Fix  

## Summary

Fix undefined behavior in `LuaTTable::getValue()` due to missing bounds checking and modernize null pointer handling by replacing deprecated `NULL` macro with `nullptr`.

## Motivation

### Issue 1: Bounds Checking Bug in `LuaTTable::getValue()`

**Current Implementation** (`Source/Engine/LuaTTable.cpp:206-208`):
```cpp
LuaType &LuaTTable::getValue(Table::Key key) {
    return *table[key];
}
```

**Problems:**
1. `std::map::operator[]` creates a default entry if the key doesn't exist
2. If the key doesn't exist, `nullptr` is inserted and dereferenced → undefined behavior
3. If the key exists with `nullptr` value → undefined behavior

**Documented Behavior** (`Source/Engine/LuaTTable.hpp:181-182`):
> "Returns the value stored at the key. If the key is not found, a nil value will be returned"

Current implementation violates this contract.

### Issue 2: Inconsistent Null Pointer Handling

**Locations using deprecated `NULL` macro:**
- `Source/LuaContext.cpp:187` - `std::shared_ptr<LuaLibrary> foundLibrary = NULL;`
- `Source/LuaContext.cpp:250` - `std::shared_ptr<Registry::LuaCFunction> builtInFnc = NULL;`
- `Source/Engine/LuaTUserData.hpp:99` - `userdata(NULL)`

**Why `nullptr` is preferred (C++11+):**
1. Type-safe: `nullptr` has type `std::nullptr_t`
2. Avoids ambiguity with integer `0`
3. Works correctly with template deduction
4. Modern C++ best practice

## Proposed Solution

### Solution 1: Bounds Checking Fix

Replace `getValue()` implementation:

```cpp
#include "LuaTNil.hpp"  // Add at top of file

LuaType &LuaTTable::getValue(Table::Key key) {
    auto it = table.find(key);
    if (it == table.end()) {
        static LuaTNil nilPlaceholder;
        return nilPlaceholder;
    }
    return *(it->second);
}
```

**Rationale:**
- Uses `find()` to avoid modifying the map
- Returns static `LuaTNil` placeholder for missing keys (matches documented behavior)
- No dynamic allocation overhead
- Thread-safe for read access (static initialization is thread-safe in C++11+)

### Solution 2: NULL → nullptr Replacement

**Source Files to Modify:**

| File | Line | Current | Change To |
|------|------|---------|-----------|
| `Source/LuaContext.cpp` | 187 | `= NULL` | `= nullptr` |
| `Source/LuaContext.cpp` | 250 | `= NULL` | `= nullptr` |
| `Source/Engine/LuaTUserData.hpp` | 99 | `userdata(NULL)` | `userdata(nullptr)` |

**Test Files to Update:**

| File | Occurrences |
|------|-------------|
| `Source/UnitTest/TestLuaContext.cpp` | 18 |
| `Source/UnitTest/TestLuaContextNewState.cpp` | 2 |
| `Source/UnitTest/TestLuaCompiler.cpp` | 2 |
| `Source/UnitTest/TestLuaMetaObject.cpp` | 2 |
| `Source/UnitTest/TestLuaTypes.cpp` | 16 |

**Pattern Change:**
```cpp
// Before
EXPECT_NE((Engine::LuaState *) NULL, L.get());

// After
EXPECT_NE(nullptr, L.get());
```

**Files NOT to Modify (C API requires NULL):**
- `Source/Registry/LuaLibrary.cpp:136-137,162-163` - `luaL_Reg` struct sentinel values
- `Source/Registry/LuaCodeSnippet.cpp:61` - `lua_load()` C API parameter

## Impact Analysis

### API Compatibility
- **Breaking:** No - return type and semantics unchanged
- **Behavior Change:** Yes - missing keys now return nil instead of UB

### Performance
- No significant performance impact
- `find()` is O(log n), same as `operator[]`
- Avoids unnecessary map insertion

### Testing
- Existing tests should pass unchanged
- No new tests required (behavior now matches documentation)
- Memory tests (`make test_memory`) should show no regressions

## Implementation Checklist

- [x] Modify `Source/Engine/LuaTTable.cpp` - fix `getValue()`
- [x] Modify `Source/LuaContext.cpp` - replace NULL with nullptr
- [x] Modify `Source/Engine/LuaTUserData.hpp` - replace NULL with nullptr
- [x] Update test files for NULL → nullptr consistency
- [x] Run `make test` to verify no regressions
- [x] Run `make test_memory` to verify no memory leaks

## References

- C++ Core Guidelines: [ES.47: Use nullptr rather than 0 or NULL](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es47-use-nullptr-rather-than-0-or-null)
- `std::map::operator[]` documentation: inserts default-constructed element if key not found