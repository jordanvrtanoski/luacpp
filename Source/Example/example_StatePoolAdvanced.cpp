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
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>
#include <chrono>

using namespace LuaCpp;
using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;

int main(int argc, char **argv) {

	std::cout << "=== LuaCpp State Pooling - Advanced Example ===" << "\n\n";

	LuaContext ctx;

	// ========================================
	// Example 1: Manual acquire/release for state reuse
	// ========================================
	std::cout << "1. Manual acquire/release for state reuse:" << "\n";
	std::cout << "   When you need to execute multiple scripts on the same state," << "\n";
	std::cout << "   manually acquire a state and keep it as long as needed." << "\n\n";

	ctx.CompileString("init_counter", "counter = 0");
	ctx.CompileString("increment_counter", "counter = counter + 1");
	ctx.CompileString("get_counter", "print('  Counter value: ' .. counter)");

	std::cout << "   Acquiring state from pool..." << "\n";
	auto state = ctx.AcquirePooledState("default");

	std::cout << "   Executing multiple scripts on the same state:" << "\n";
	try {
		auto& pool = ctx.getPool("default");
		std::cout << "   Pool stats (while state is checked out):" << "\n";
		std::cout << "     - Available: " << pool.availableCount() << "\n";
		std::cout << "     - Checked out: " << pool.checkedOutCount() << "\n\n";

		ctx.RunWithEnvironmentPooled("init_counter", LuaEnvironment(), "default");
		ctx.RunWithEnvironmentPooled("increment_counter", LuaEnvironment(), "default");
		ctx.RunWithEnvironmentPooled("increment_counter", LuaEnvironment(), "default");
		ctx.RunWithEnvironmentPooled("increment_counter", LuaEnvironment(), "default");
		ctx.RunWithEnvironmentPooled("get_counter", LuaEnvironment(), "default");
	}
	catch (std::runtime_error& e) {
		std::cout << "   Error: " << e.what() << "\n";
	}

	std::cout << "\n   Releasing state back to pool..." << "\n";
	ctx.ReleasePooledState(std::move(state), "default");

	{
		auto& pool = ctx.getPool("default");
		std::cout << "   Pool stats (after state is released):" << "\n";
		std::cout << "     - Available: " << pool.availableCount() << "\n";
		std::cout << "     - Checked out: " << pool.checkedOutCount() << "\n";
	}
	std::cout << "\n";

	// ========================================
	// Example 2: RAII wrapper for exception safety
	// ========================================
	std::cout << "2. RAII wrapper (PooledState) for exception safety:" << "\n";
	std::cout << "   The PooledState wrapper automatically returns the state" << "\n";
	std::cout << "   to the pool when it goes out of scope, even if an exception occurs." << "\n\n";

	ctx.CompileString("raii_test", "print('  Executing with RAII wrapper')");

	{
		std::cout << "   Entering scope..." << "\n";
		auto pooledState = ctx.AcquirePooledStateRAII("default");

		auto& pool = ctx.getPool("default");
		std::cout << "   Pool stats (inside scope):" << "\n";
		std::cout << "     - Available: " << pool.availableCount() << "\n";
		std::cout << "     - Checked out: " << pool.checkedOutCount() << "\n";

		try {
			ctx.RunPooled("raii_test");
		}
		catch (std::runtime_error& e) {
			std::cout << "   Error: " << e.what() << "\n";
		}

		std::cout << "   Exiting scope (state will be auto-released)..." << "\n";
	}

	{
		auto& pool = ctx.getPool("default");
		std::cout << "   Pool stats (after scope exit):" << "\n";
		std::cout << "     - Available: " << pool.availableCount() << "\n";
		std::cout << "     - Checked out: " << pool.checkedOutCount() << "\n";
	}
	std::cout << "\n";

	// ========================================
	// Example 3: Multiple pool colors for different scenarios
	// ========================================
	std::cout << "3. Multiple pool colors for different scenarios:" << "\n";
	std::cout << "   Use different pool colors for scripts with different requirements." << "\n\n";

	ctx.CompileString("calc", "print('  Result: ' .. math.sqrt(144))");
	ctx.CompileString("file_op", "print('  Note: io library available for file operations')");

	std::cout << "   Running calculation on 'sandboxed' pool (has math, no io):" << "\n";
	try {
		ctx.RunPooled("calc", "sandboxed");
	}
	catch (std::runtime_error& e) {
		std::cout << "   Error: " << e.what() << "\n";
	}

	std::cout << "\n   Running on 'io' pool (has io and os):" << "\n";
	try {
		ctx.RunPooled("file_op", "io");
	}
	catch (std::runtime_error& e) {
		std::cout << "   Error: " << e.what() << "\n";
	}

	std::cout << "\n   Listing all available pools:" << "\n";
	PoolManager& poolManager = ctx.getPoolManager();
	for (const auto& color : poolManager.listPools()) {
		std::cout << "     - " << color << "\n";
	}
	std::cout << "\n";

	// ========================================
	// Example 4: Thread-safe pool with concurrent access
	// ========================================
	std::cout << "4. Thread-safe pool with concurrent access:" << "\n";
	std::cout << "   Enable thread safety to safely share a pool across multiple threads." << "\n\n";

	ctx.CompileString("thread_work", "print('  Thread working... result = ' .. math.sqrt(100))");

	StatePool& threadPool = ctx.createPool("thread_safe", PoolConfig().SetMaxSize(3));
	threadPool.setThreadSafe(true);
	threadPool.warmup(3);

	std::cout << "   Spawning 3 threads using the same pool..." << "\n";
	std::cout << "   Pool max size: " << threadPool.getMaxSize() << "\n\n";

	std::vector<std::thread> threads;
	for (int i = 0; i < 3; i++) {
		threads.emplace_back([&ctx, i]() {
			try {
				auto state = ctx.AcquirePooledState("thread_safe");
				std::this_thread::sleep_for(std::chrono::milliseconds(50 * i));
				ctx.RunPooled("thread_work", "thread_safe");
				ctx.ReleasePooledState(std::move(state), "thread_safe");
			}
			catch (const PoolExhaustedException& e) {
				std::cout << "   Thread " << i << ": " << e.what() << "\n";
			}
			catch (const std::runtime_error& e) {
				std::cout << "   Thread " << i << " error: " << e.what() << "\n";
			}
		});
	}

	for (auto& t : threads) {
		t.join();
	}
	std::cout << "\n";

	// ========================================
	// Example 5: Pool exhaustion exception handling
	// ========================================
	std::cout << "5. Pool exhaustion exception handling:" << "\n";
	std::cout << "   When the pool is full and all states are checked out," << "\n";
	std::cout << "   PoolExhaustedException is thrown." << "\n\n";

	PoolConfig smallConfig;
	smallConfig.maxSize = 1;
	ctx.createPool("tiny", smallConfig);

	auto& tinyPool = ctx.getPool("tiny");
	std::cout << "   Created 'tiny' pool with max size = 1" << "\n";

	auto tinyState = ctx.AcquirePooledState("tiny");
	std::cout << "   Acquired 1 state (pool now empty)..." << "\n";
	std::cout << "   Pool stats:" << "\n";
	std::cout << "     - Available: " << tinyPool.availableCount() << "\n";
	std::cout << "     - Checked out: " << tinyPool.checkedOutCount() << "\n";
	std::cout << "     - Max size: " << tinyPool.getMaxSize() << "\n\n";

	std::cout << "   Attempting to acquire another state..." << "\n";
	try {
		auto anotherState = ctx.AcquirePooledState("tiny");
		std::cout << "   Unexpectedly succeeded!" << "\n";
	}
	catch (const PoolExhaustedException& e) {
		std::cout << "   Caught PoolExhaustedException as expected:" << "\n";
		std::cout << "     " << e.what() << "\n";
	}

	std::cout << "\n   Releasing state back to pool..." << "\n";
	ctx.ReleasePooledState(std::move(tinyState), "tiny");
	std::cout << "   Now available: " << tinyPool.availableCount() << "\n";
	std::cout << "\n";

	// ========================================
	// Example 6: Draining a pool
	// ========================================
	std::cout << "6. Draining a pool:" << "\n";
	std::cout << "   Use drain() to remove all available states from the pool." << "\n\n";

	StatePool& drainPool = ctx.createPool("drain_test", PoolConfig().SetMaxSize(5));
	drainPool.warmup(4);

	std::cout << "   Pool after warmup(4):" << "\n";
	std::cout << "     - Current size: " << drainPool.getCurrentSize() << "\n";
	std::cout << "     - Available: " << drainPool.availableCount() << "\n";

	drainPool.drain();

	std::cout << "\n   Pool after drain():" << "\n";
	std::cout << "     - Current size: " << drainPool.getCurrentSize() << "\n";
	std::cout << "     - Available: " << drainPool.availableCount() << "\n";
	std::cout << "\n";

	std::cout << "=== Example complete ===" << "\n";

	return 0;
}
