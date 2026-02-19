/*
   MIT License

   Copyright (c) 2021 Jordan Vrtanoski

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
   */

#include "../LuaCpp.hpp"
#include "gtest/gtest.h"
#include "PoolTestUtils.hpp"

using namespace LuaCpp;
using namespace LuaCpp::Engine;
using namespace LuaCpp::Test;

extern "C" {
	static void testHook(lua_State *L, lua_Debug *ar) {
		(void)L;
		(void)ar;
	}
}

class TestStatePool : public ::testing::Test {
protected:
	void SetUp() override {
	}

	void TearDown() override {
	}

	void VerifyAcquireReleaseCounts(StatePool& pool, size_t expectedSize) {
		EXPECT_EQ(expectedSize, pool.getCurrentSize());
		EXPECT_EQ(0u, pool.availableCount());
		EXPECT_EQ(expectedSize, pool.checkedOutCount());
	}

	void VerifyAvailableCounts(StatePool& pool, size_t expectedSize, size_t expectedAvailable) {
		EXPECT_EQ(expectedSize, pool.getCurrentSize());
		EXPECT_EQ(expectedAvailable, pool.availableCount());
		EXPECT_EQ(0u, pool.checkedOutCount());
	}

	void AcquireVerifyReleaseThree(StatePool& pool) {
		auto state1 = pool.acquire();
		auto state2 = pool.acquire();
		auto state3 = pool.acquire();

		VerifyAcquireReleaseCounts(pool, 3u);

		pool.release(std::move(state1));
		pool.release(std::move(state2));
		pool.release(std::move(state3));
	}
};

TEST_F(TestStatePool, CreatePoolWithDefaultConfig) {
	PoolConfig config;
	config.maxSize = 3;

	StatePool pool("test", config);

	EXPECT_EQ("test", pool.getColor());
	EXPECT_EQ(3u, pool.getMaxSize());
	EXPECT_EQ(0u, pool.getCurrentSize());
	EXPECT_EQ(0u, pool.availableCount());
	EXPECT_EQ(0u, pool.checkedOutCount());
	EXPECT_FALSE(pool.isThreadSafe());
}

TEST_F(TestStatePool, AcquireAndReleaseState) {
	PoolConfig config;
	config.maxSize = 2;

	StatePool pool("test", config);

	auto state = pool.acquire();
	EXPECT_NE(nullptr, state.get());
	VerifyAcquireReleaseCounts(pool, 1u);

	pool.release(std::move(state));
	VerifyAvailableCounts(pool, 1u, 1u);
}

TEST_F(TestStatePool, AcquireMultipleStates) {
	PoolConfig config;
	config.maxSize = 3;

	StatePool pool("test", config);

	AcquireVerifyReleaseThree(pool);

	VerifyAvailableCounts(pool, 3u, 3u);
}

TEST_F(TestStatePool, ReuseAvailableState) {
	PoolConfig config;
	config.maxSize = 2;

	StatePool pool("test", config);

	auto state1 = pool.acquire();
	lua_pushstring(*state1, "test_data");
	pool.release(std::move(state1));

	EXPECT_EQ(1u, pool.availableCount());

	auto state2 = pool.acquire();
	EXPECT_EQ(0u, lua_gettop(*state2));
	
	pool.release(std::move(state2));
}

TEST_F(TestStatePool, PoolExhausted) {
	PoolConfig config;
	config.maxSize = 1;
	config.exhaustionTimeoutMs = 1;

	StatePool pool("test", config);

	auto state1 = pool.acquire();
	EXPECT_NE(nullptr, state1.get());

	EXPECT_THROW(pool.acquire(), PoolExhaustedException);

	pool.release(std::move(state1));
}

TEST_F(TestStatePool, WarmupPool) {
	PoolConfig config;
	config.maxSize = 5;

	StatePool pool("test", config);
	pool.warmup(3);

	VerifyAvailableCounts(pool, 3u, 3u);

	AcquireVerifyReleaseThree(pool);

	VerifyAvailableCounts(pool, 3u, 3u);
}

TEST_F(TestStatePool, WarmupDoesNotExceedMaxSize) {
	PoolConfig config;
	config.maxSize = 2;

	StatePool pool("test", config);
	pool.warmup(5);

	VerifyAvailableCounts(pool, 2u, 2u);
}

TEST_F(TestStatePool, DrainPool) {
	PoolConfig config;
	config.maxSize = 3;

	StatePool pool("test", config);
	pool.warmup(3);

	VerifyAvailableCounts(pool, 3u, 3u);

	pool.drain();

	VerifyAvailableCounts(pool, 0u, 0u);
}

TEST_F(TestStatePool, SetThreadSafe) {
	PoolConfig config;
	StatePool pool("test", config);

	EXPECT_FALSE(pool.isThreadSafe());

	pool.setThreadSafe(true);
	EXPECT_TRUE(pool.isThreadSafe());

	pool.setThreadSafe(false);
	EXPECT_FALSE(pool.isThreadSafe());
}

TEST_F(TestStatePool, AcquireFromAvailableQueue) {
	PoolConfig config;
	config.maxSize = 3;

	StatePool pool("test", config);

	pool.warmup(2);
	EXPECT_EQ(2u, pool.availableCount());

	auto state1 = pool.acquire();
	EXPECT_EQ(1u, pool.availableCount());
	EXPECT_EQ(2u, pool.getCurrentSize());

	auto state2 = pool.acquire();
	EXPECT_EQ(0u, pool.availableCount());

	auto state3 = pool.acquire();
	EXPECT_EQ(3u, pool.getCurrentSize());

	pool.release(std::move(state1));
	pool.release(std::move(state2));
	pool.release(std::move(state3));
}

TEST_F(TestStatePool, PoolConfigWithLibraries) {
	PoolConfig config;
	config.libraries = {"base", "math"};
	config.maxSize = 2;

	StatePool pool("test", config);

	auto state = pool.acquire();
	EXPECT_NE(nullptr, state.get());

	ExpectGlobalIsTable(*state, "math");
	ExpectGlobalIsNil(*state, "io");
	ExpectGlobalIsNil(*state, "os");
	ExpectGlobalIsNil(*state, "string");
	ExpectGlobalIsFunction(*state, "print");

	pool.release(std::move(state));
}

TEST_F(TestStatePool, PoolConfigWithGlobalVariables) {
	PoolConfig config;
	config.maxSize = 2;
	config.AddGlobalVariable("test_var", std::make_shared<LuaTNumber>(42.0));

	StatePool pool("test", config);

	auto state = pool.acquire();
	
	ExpectGlobalIsNumber(*state, "test_var", 42.0);

	pool.release(std::move(state));
}

TEST_F(TestStatePool, PoolConfigWithHooks) {
	PoolConfig config;
	config.maxSize = 2;
	config.AddHook("call", 0, testHook);

	StatePool pool("test", config);

	auto state = pool.acquire();
	EXPECT_NE(nullptr, state.get());
	
	pool.release(std::move(state));
}

TEST_F(TestStatePool, ThreadSafeAcquireAndRelease) {
	PoolConfig config;
	config.maxSize = 3;

	StatePool pool("test", config);
	pool.setThreadSafe(true);

	auto state1 = pool.acquire();
	auto state2 = pool.acquire();

	VerifyAcquireReleaseCounts(pool, 2u);

	pool.release(std::move(state1));
	pool.release(std::move(state2));

	VerifyAvailableCounts(pool, 2u, 2u);
}

TEST_F(TestStatePool, GetConfig) {
	PoolConfig config;
	config.maxSize = 7;

	StatePool pool("test", config);

	const PoolConfig& retrievedConfig = pool.getConfig();
	EXPECT_EQ(7u, retrievedConfig.maxSize);
}

TEST_F(TestStatePool, EmptyLibrariesLoadsAll) {
	PoolConfig config;
	config.libraries = {};
	config.maxSize = 1;

	StatePool pool("test", config);

	auto state = pool.acquire();
	
	ExpectAllStandardLibraries(*state);

	pool.release(std::move(state));
}

TEST_F(TestStatePool, PoolConfigFluentAPI) {
	PoolConfig config = PoolConfig()
		.SetLibraries({"base"})
		.SetMaxSize(3)
		.SetExhaustionTimeoutMs(100);

	EXPECT_EQ(3u, config.maxSize);
	EXPECT_EQ(100u, config.exhaustionTimeoutMs);
	EXPECT_EQ(1u, config.libraries.size());
}

TEST_F(TestStatePool, PoolConfigWithLibrariesWithoutBase) {
	PoolConfig config;
	config.libraries = {"math"};
	config.maxSize = 2;

	StatePool pool("test", config);

	auto state = pool.acquire();
	EXPECT_NE(nullptr, state.get());

	ExpectGlobalIsTable(*state, "math");
	ExpectGlobalIsFunction(*state, "print");

	pool.release(std::move(state));
}

TEST_F(TestStatePool, PoolConfigWithHookTypes) {
	PoolConfig config;
	config.maxSize = 4;
	config.AddHook("call", 0, testHook);
	config.AddHook("return", 0, testHook);
	config.AddHook("line", 0, testHook);
	config.AddHook("count", 1, testHook);

	StatePool pool("test", config);

	auto state = pool.acquire();
	EXPECT_NE(nullptr, state.get());
	
	luaL_dostring(*state, "function test() return 1 end test()");
	
	pool.release(std::move(state));
}

TEST_F(TestStatePool, AllStandardLibraries) {
	PoolConfig config;
	config.libraries = {"base", "coroutine", "debug", "io", "math", "os", "package", "string", "table", "utf8"};
	config.maxSize = 1;

	StatePool pool("test", config);

	auto state = pool.acquire();
	EXPECT_NE(nullptr, state.get());

	ExpectAllStandardLibraries(*state);

	pool.release(std::move(state));
}

TEST_F(TestStatePool, ThreadSafeWarmup) {
	PoolConfig config;
	config.maxSize = 3;

	StatePool pool("test", config);
	pool.setThreadSafe(true);
	
	EXPECT_EQ(0u, pool.getCurrentSize());
	
	pool.warmup(2);
	
	VerifyAvailableCounts(pool, 2u, 2u);
}

TEST_F(TestStatePool, ThreadSafeStatsMethods) {
	PoolConfig config;
	config.maxSize = 3;

	StatePool pool("test", config);
	pool.setThreadSafe(true);
	
	EXPECT_EQ(0u, pool.getCurrentSize());
	EXPECT_EQ(0u, pool.availableCount());
	
	pool.warmup(2);
	
	EXPECT_EQ(2u, pool.getCurrentSize());
	EXPECT_EQ(2u, pool.availableCount());
	
	auto state = pool.acquire();
	
	EXPECT_EQ(2u, pool.getCurrentSize());
	EXPECT_EQ(1u, pool.availableCount());
	EXPECT_EQ(1u, pool.checkedOutCount());
	
	pool.release(std::move(state));
}

TEST_F(TestStatePool, ThreadSafeDrain) {
	PoolConfig config;
	config.maxSize = 3;

	StatePool pool("test", config);
	pool.setThreadSafe(true);
	
	pool.warmup(3);
	EXPECT_EQ(3u, pool.getCurrentSize());
	
	pool.drain();
	
	VerifyAvailableCounts(pool, 0u, 0u);
}

TEST_F(TestStatePool, AcquireFromAvailableQueueThreadSafe) {
	PoolConfig config;
	config.maxSize = 3;

	StatePool pool("test", config);
	pool.setThreadSafe(true);

	pool.warmup(2);
	EXPECT_EQ(2u, pool.availableCount());

	auto state1 = pool.acquire();
	EXPECT_EQ(1u, pool.availableCount());
	EXPECT_EQ(2u, pool.getCurrentSize());

	pool.release(std::move(state1));
	EXPECT_EQ(2u, pool.availableCount());

	auto state2 = pool.acquire();
	EXPECT_EQ(1u, pool.availableCount());
	
	pool.release(std::move(state2));
}
