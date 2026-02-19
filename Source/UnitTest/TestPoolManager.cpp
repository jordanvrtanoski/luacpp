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

class TestPoolManager : public ::testing::Test {
protected:
	void SetUp() override {
	}

	void TearDown() override {
	}

	void VerifyPredefinedPools(PoolManager& manager) {
		EXPECT_TRUE(manager.hasPool("default"));
		EXPECT_TRUE(manager.hasPool("sandboxed"));
		EXPECT_TRUE(manager.hasPool("minimal"));
		EXPECT_TRUE(manager.hasPool("io"));
	}
};

TEST_F(TestPoolManager, CreateManagerWithPredefinedPools) {
	PoolManager manager;
	VerifyPredefinedPools(manager);
}

TEST_F(TestPoolManager, GetPool) {
	PoolManager manager;

	StatePool& defaultPool = manager.getPool("default");
	EXPECT_EQ("default", defaultPool.getColor());
	EXPECT_EQ(5u, defaultPool.getMaxSize());

	StatePool& sandboxedPool = manager.getPool("sandboxed");
	EXPECT_EQ("sandboxed", sandboxedPool.getColor());

	StatePool& minimalPool = manager.getPool("minimal");
	EXPECT_EQ("minimal", minimalPool.getColor());

	StatePool& ioPool = manager.getPool("io");
	EXPECT_EQ("io", ioPool.getColor());
}

TEST_F(TestPoolManager, GetPoolNotFound) {
	PoolManager manager;

	EXPECT_THROW(manager.getPool("nonexistent"), std::runtime_error);
}

TEST_F(TestPoolManager, CreateCustomPool) {
	PoolManager manager;

	PoolConfig config;
	config.maxSize = 3;
	config.libraries = {"base", "math"};

	StatePool& customPool = manager.createPool("custom", config);
	EXPECT_EQ("custom", customPool.getColor());
	EXPECT_EQ(3u, customPool.getMaxSize());

	EXPECT_TRUE(manager.hasPool("custom"));
}

TEST_F(TestPoolManager, CreateDuplicatePool) {
	PoolManager manager;

	PoolConfig config;
	EXPECT_NO_THROW(manager.createPool("custom1", config));
	EXPECT_THROW(manager.createPool("custom1", config), std::runtime_error);
}

TEST_F(TestPoolManager, DestroyCustomPool) {
	PoolManager manager;

	PoolConfig config;
	manager.createPool("custom2", config);
	EXPECT_TRUE(manager.hasPool("custom2"));

	EXPECT_NO_THROW(manager.destroyPool("custom2"));
	EXPECT_FALSE(manager.hasPool("custom2"));
}

TEST_F(TestPoolManager, DestroyNonExistentPool) {
	PoolManager manager;

	EXPECT_THROW(manager.destroyPool("nonexistent"), std::runtime_error);
}

TEST_F(TestPoolManager, CannotDestroyPredefinedPool) {
	PoolManager manager;

	EXPECT_THROW(manager.destroyPool("default"), std::runtime_error);
	EXPECT_THROW(manager.destroyPool("sandboxed"), std::runtime_error);
	EXPECT_THROW(manager.destroyPool("minimal"), std::runtime_error);
	EXPECT_THROW(manager.destroyPool("io"), std::runtime_error);
}

TEST_F(TestPoolManager, ListPools) {
	PoolManager manager;

	auto pools = manager.listPools();
	EXPECT_EQ(4u, pools.size());

	bool hasDefault = false, hasSandboxed = false, hasMinimal = false, hasIO = false;
	for (const auto& pool : pools) {
		if (pool == "default") hasDefault = true;
		if (pool == "sandboxed") hasSandboxed = true;
		if (pool == "minimal") hasMinimal = true;
		if (pool == "io") hasIO = true;
	}
	EXPECT_TRUE(hasDefault);
	EXPECT_TRUE(hasSandboxed);
	EXPECT_TRUE(hasMinimal);
	EXPECT_TRUE(hasIO);

	PoolConfig config;
	manager.createPool("custom3", config);
	pools = manager.listPools();
	EXPECT_EQ(5u, pools.size());
}

TEST_F(TestPoolManager, SetThreadSafe) {
	PoolManager manager;

	EXPECT_FALSE(manager.isThreadSafe());

	manager.setThreadSafe(true);
	EXPECT_TRUE(manager.isThreadSafe());

	StatePool& defaultPool = manager.getPool("default");
	EXPECT_TRUE(defaultPool.isThreadSafe());

	StatePool& sandboxedPool = manager.getPool("sandboxed");
	EXPECT_TRUE(sandboxedPool.isThreadSafe());

	manager.setThreadSafe(false);
	EXPECT_FALSE(manager.isThreadSafe());
	EXPECT_FALSE(defaultPool.isThreadSafe());
	EXPECT_FALSE(sandboxedPool.isThreadSafe());
}

TEST_F(TestPoolManager, NewPoolsInheritThreadSafety) {
	PoolManager manager;
	manager.setThreadSafe(true);

	PoolConfig config;
	StatePool& customPool = manager.createPool("custom_threadsafe", config);
	EXPECT_TRUE(customPool.isThreadSafe());
}

TEST_F(TestPoolManager, AcquireFromPredefinedPool) {
	PoolManager manager;

	StatePool& defaultPool = manager.getPool("default");
	auto state = defaultPool.acquire();
	EXPECT_NE(nullptr, state.get());
	
	ExpectDefaultLibraries(*state);
	
	defaultPool.release(std::move(state));

	StatePool& sandboxedPool = manager.getPool("sandboxed");
	state = sandboxedPool.acquire();
	EXPECT_NE(nullptr, state.get());
	
	ExpectSandboxedLibraries(*state);
	
	sandboxedPool.release(std::move(state));

	StatePool& minimalPool = manager.getPool("minimal");
	state = minimalPool.acquire();
	EXPECT_NE(nullptr, state.get());
	
	ExpectBaseLibrariesOnly(*state);
	
	minimalPool.release(std::move(state));

	StatePool& ioPool = manager.getPool("io");
	state = ioPool.acquire();
	EXPECT_NE(nullptr, state.get());
	
	ExpectIOLibraries(*state);
	
	ioPool.release(std::move(state));
}

TEST_F(TestPoolManager, HasPoolReturnsCorrectly) {
	PoolManager manager;

	EXPECT_TRUE(manager.hasPool("default"));
	EXPECT_FALSE(manager.hasPool("nonexistent"));

	PoolConfig config;
	manager.createPool("test_pool", config);
	EXPECT_TRUE(manager.hasPool("test_pool"));
}

TEST_F(TestPoolManager, ThreadSafeDestroyPool) {
	PoolManager manager;
	manager.setThreadSafe(true);

	PoolConfig config;
	manager.createPool("custom_to_destroy", config);
	EXPECT_TRUE(manager.hasPool("custom_to_destroy"));

	EXPECT_NO_THROW(manager.destroyPool("custom_to_destroy"));
	EXPECT_FALSE(manager.hasPool("custom_to_destroy"));
}

TEST_F(TestPoolManager, ThreadSafeHasPool) {
	PoolManager manager;
	manager.setThreadSafe(true);

	EXPECT_TRUE(manager.hasPool("default"));
	EXPECT_FALSE(manager.hasPool("nonexistent"));

	PoolConfig config;
	manager.createPool("ts_test", config);
	EXPECT_TRUE(manager.hasPool("ts_test"));
}

TEST_F(TestPoolManager, ThreadSafeListPools) {
	PoolManager manager;
	manager.setThreadSafe(true);

	auto pools = manager.listPools();
	EXPECT_EQ(4u, pools.size());
}
