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

using namespace LuaCpp;
using namespace LuaCpp::Engine;

class TestLuaContextPooling : public ::testing::Test {
protected:
	void SetUp() override {
	}

	void TearDown() override {
	}
};

TEST_F(TestLuaContextPooling, GetPoolManager) {
	LuaContext ctx;

	PoolManager& manager = ctx.getPoolManager();
	EXPECT_TRUE(manager.hasPool("default"));
	EXPECT_TRUE(manager.hasPool("sandboxed"));
	EXPECT_TRUE(manager.hasPool("minimal"));
	EXPECT_TRUE(manager.hasPool("io"));
}

TEST_F(TestLuaContextPooling, GetPool) {
	LuaContext ctx;

	StatePool& pool = ctx.getPool("default");
	EXPECT_EQ("default", pool.getColor());
	EXPECT_EQ(5u, pool.getMaxSize());
}

TEST_F(TestLuaContextPooling, HasPool) {
	LuaContext ctx;

	EXPECT_TRUE(ctx.hasPool("default"));
	EXPECT_TRUE(ctx.hasPool("sandboxed"));
	EXPECT_FALSE(ctx.hasPool("nonexistent"));
}

TEST_F(TestLuaContextPooling, CreatePool) {
	LuaContext ctx;

	PoolConfig config;
	config.maxSize = 3;

	StatePool& pool = ctx.createPool("custom", config);
	EXPECT_EQ("custom", pool.getColor());
	EXPECT_EQ(3u, pool.getMaxSize());

	EXPECT_TRUE(ctx.hasPool("custom"));
}

TEST_F(TestLuaContextPooling, RunPooled) {
	LuaContext ctx;

	ctx.CompileString("hello", "print('Hello from pooled state')");

	testing::internal::CaptureStdout();
	ctx.RunPooled("hello");
	std::string output = testing::internal::GetCapturedStdout();

	EXPECT_TRUE(output.find("Hello from pooled state") != std::string::npos);
}

TEST_F(TestLuaContextPooling, RunPooledWithDifferentColor) {
	LuaContext ctx;

	ctx.CompileString("math_test", "result = math.sqrt(16)");

	testing::internal::CaptureStdout();
	EXPECT_NO_THROW(ctx.RunPooled("math_test", "sandboxed"));
}

TEST_F(TestLuaContextPooling, RunPooledNonExistentSnippet) {
	LuaContext ctx;

	EXPECT_THROW(ctx.RunPooled("nonexistent"), std::runtime_error);
}

TEST_F(TestLuaContextPooling, RunWithEnvironmentPooled) {
	LuaContext ctx;

	ctx.CompileString("env_test", "test_var = test_var + 1");

	auto numVar = std::make_shared<LuaTNumber>(10.0);
	LuaEnvironment env;
	env["test_var"] = numVar;

	ctx.RunWithEnvironmentPooled("env_test", env);

	EXPECT_DOUBLE_EQ(11.0, std::dynamic_pointer_cast<LuaTNumber>(env["test_var"])->getValue());
}

TEST_F(TestLuaContextPooling, AcquireAndReleasePooledState) {
	LuaContext ctx;

	auto state = ctx.AcquirePooledState("default");
	EXPECT_NE(nullptr, state.get());

	lua_pushstring(*state, "test");
	EXPECT_EQ(1, lua_gettop(*state));

	ctx.ReleasePooledState(std::move(state), "default");
}

TEST_F(TestLuaContextPooling, AcquirePooledStateMultipleTimes) {
	LuaContext ctx;

	auto& pool = ctx.getPool("default");
	pool.warmup(2);

	auto state1 = ctx.AcquirePooledState("default");
	auto state2 = ctx.AcquirePooledState("default");

	EXPECT_NE(nullptr, state1.get());
	EXPECT_NE(nullptr, state2.get());

	ctx.ReleasePooledState(std::move(state1), "default");
	ctx.ReleasePooledState(std::move(state2), "default");
}

TEST_F(TestLuaContextPooling, AcquirePooledStateRAII) {
	LuaContext ctx;

	{
		auto pooledState = ctx.AcquirePooledStateRAII("default");
		EXPECT_NE(nullptr, pooledState.get());

		lua_pushstring(*pooledState, "test");
		EXPECT_EQ(1, lua_gettop(*pooledState));
	}

	auto& pool = ctx.getPool("default");
	EXPECT_EQ(1u, pool.availableCount());
}

TEST_F(TestLuaContextPooling, PooledStateDestructorReleasesState) {
	LuaContext ctx;

	{
		auto pooledState = ctx.AcquirePooledStateRAII("default");
		EXPECT_NE(nullptr, pooledState.get());
		lua_pushstring(*pooledState, "test");
	}

	auto& pool = ctx.getPool("default");
	EXPECT_EQ(1u, pool.availableCount());
}

TEST_F(TestLuaContextPooling, PooledStateMoveConstructor) {
	LuaContext ctx;

	auto pooled1 = ctx.AcquirePooledStateRAII("default");
	EXPECT_TRUE(static_cast<bool>(pooled1));

	auto pooled2 = std::move(pooled1);
	EXPECT_TRUE(static_cast<bool>(pooled2));
	EXPECT_FALSE(static_cast<bool>(pooled1));
}

TEST_F(TestLuaContextPooling, PooledStateMoveAssignment) {
	LuaContext ctx;

	auto pooled1 = ctx.AcquirePooledStateRAII("default");
	auto pooled2 = ctx.AcquirePooledStateRAII("default");

	EXPECT_TRUE(static_cast<bool>(pooled1));
	EXPECT_TRUE(static_cast<bool>(pooled2));

	pooled1 = std::move(pooled2);

	EXPECT_TRUE(static_cast<bool>(pooled1));
	EXPECT_FALSE(static_cast<bool>(pooled2));
}

TEST_F(TestLuaContextPooling, PooledStateDereferenceOperators) {
	LuaContext ctx;

	auto pooled = ctx.AcquirePooledStateRAII("default");

	EXPECT_NE(nullptr, &(*pooled));
	EXPECT_NE(nullptr, pooled.get());
	EXPECT_NE(nullptr, pooled.operator->());
}

TEST_F(TestLuaContextPooling, PooledStateExplicitRelease) {
	LuaContext ctx;

	auto pooled = ctx.AcquirePooledStateRAII("default");
	EXPECT_TRUE(static_cast<bool>(pooled));

	pooled.release();
	EXPECT_FALSE(static_cast<bool>(pooled));
}

TEST_F(TestLuaContextPooling, PooledStateBoolConversion) {
	LuaContext ctx;

	auto pooled = ctx.AcquirePooledStateRAII("default");
	EXPECT_TRUE(static_cast<bool>(pooled));

	pooled.release();
	EXPECT_FALSE(static_cast<bool>(pooled));
}

TEST_F(TestLuaContextPooling, RunPooledExecutionError) {
	LuaContext ctx;

	ctx.CompileString("error_test", "error('test error')");

	EXPECT_THROW(ctx.RunPooled("error_test"), std::runtime_error);
}

TEST_F(TestLuaContextPooling, PoolReuseAfterRelease) {
	LuaContext ctx;

	ctx.CompileString("print_test", "print('pooled')");

	testing::internal::CaptureStdout();
	ctx.RunPooled("print_test");
	ctx.RunPooled("print_test");
	std::string output = testing::internal::GetCapturedStdout();

	EXPECT_TRUE(output.find("pooled") != std::string::npos);
}

TEST_F(TestLuaContextPooling, ManualStateReuse) {
	LuaContext ctx;

	ctx.CompileString("init", "counter = 0");
	ctx.CompileString("increment", "counter = counter + 1");
	ctx.CompileString("get_counter", "return counter or 0");

	auto state = ctx.AcquirePooledState("default");

	ctx.RunWithEnvironmentPooled("init", LuaEnvironment(), "default");

	for (int i = 0; i < 5; i++) {
		auto state2 = ctx.AcquirePooledState("default");
		lua_pushstring(*state2, "test");
		ctx.ReleasePooledState(std::move(state2), "default");
	}

	ctx.ReleasePooledState(std::move(state), "default");
}

TEST_F(TestLuaContextPooling, RunWithEnvironmentPooledRuntimeError) {
	LuaContext ctx;

	ctx.CompileString("error_env", "error('env error')");

	LuaEnvironment env;
	EXPECT_THROW(ctx.RunWithEnvironmentPooled("error_env", env), std::runtime_error);
}

TEST_F(TestLuaContextPooling, MultiplePoolColors) {
	LuaContext ctx;

	ctx.CompileString("lib_check", "");

	EXPECT_NO_THROW(ctx.RunPooled("lib_check", "default"));
	EXPECT_NO_THROW(ctx.RunPooled("lib_check", "sandboxed"));
	EXPECT_NO_THROW(ctx.RunPooled("lib_check", "minimal"));
	EXPECT_NO_THROW(ctx.RunPooled("lib_check", "io"));
}

TEST_F(TestLuaContextPooling, CustomPoolUsage) {
	LuaContext ctx;

	PoolConfig config;
	config.maxSize = 2;
	config.libraries = {"base", "math"};

	ctx.createPool("math_only", config);

	ctx.CompileString("math_op", "result = math.sqrt(100)");
	EXPECT_NO_THROW(ctx.RunPooled("math_op", "math_only"));
}
