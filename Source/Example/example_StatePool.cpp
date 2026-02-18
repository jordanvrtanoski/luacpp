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

using namespace LuaCpp;
using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;

int main(int argc, char **argv) {

	std::cout << "=== LuaCpp State Pooling - Basic Example ===" << "\n\n";

	LuaContext ctx;

	// ========================================
	// Example 1: Simple RunPooled() usage
	// ========================================
	std::cout << "1. Simple RunPooled() usage:" << "\n";
	std::cout << "   RunPooled() automatically acquires a state from the pool," << "\n";
	std::cout << "   executes the script, and returns the state to the pool." << "\n\n";

	ctx.CompileString("hello", "print('  Hello from pooled state!')");

	for (int i = 0; i < 3; i++) {
		std::cout << "   Execution " << (i + 1) << ":" << "\n";
		try {
			ctx.RunPooled("hello");
		}
		catch (std::runtime_error& e) {
			std::cout << "   Error: " << e.what() << "\n";
		}
	}
	std::cout << "\n";

	// ========================================
	// Example 2: Pool warmup for pre-allocation
	// ========================================
	std::cout << "2. Pool warmup for pre-allocation:" << "\n";
	std::cout << "   Warmup pre-creates states in the pool, eliminating" << "\n";
	std::cout << "   allocation overhead during execution." << "\n\n";

	StatePool& defaultPool = ctx.getPool("default");
	
	std::cout << "   Pool stats before warmup:" << "\n";
	std::cout << "     - Current size: " << defaultPool.getCurrentSize() << "\n";
	std::cout << "     - Available: " << defaultPool.availableCount() << "\n";
	std::cout << "     - Max size: " << defaultPool.getMaxSize() << "\n\n";

	defaultPool.warmup(3);
	
	std::cout << "   Pool stats after warmup(3):" << "\n";
	std::cout << "     - Current size: " << defaultPool.getCurrentSize() << "\n";
	std::cout << "     - Available: " << defaultPool.availableCount() << "\n\n";

	// ========================================
	// Example 3: Custom pool creation
	// ========================================
	std::cout << "3. Custom pool creation with specific libraries:" << "\n";
	std::cout << "   Create a pool that only loads specific Lua libraries." << "\n\n";

	PoolConfig mathOnlyConfig;
	mathOnlyConfig.libraries = {"base", "math"};
	mathOnlyConfig.maxSize = 2;

	StatePool& mathPool = ctx.createPool("math_only", mathOnlyConfig);

	ctx.CompileString("math_test", "print('  math.sqrt(16) = ' .. math.sqrt(16))");

	std::cout << "   Running on 'math_only' pool (has math library):" << "\n";
	try {
		ctx.RunPooled("math_test", "math_only");
	}
	catch (std::runtime_error& e) {
		std::cout << "   Error: " << e.what() << "\n";
	}
	std::cout << "\n";

	// ========================================
	// Example 4: Using predefined pool colors
	// ========================================
	std::cout << "4. Using predefined pool colors:" << "\n\n";

	std::cout << "   Available predefined pools:" << "\n";
	std::cout << "     - 'default': All standard libraries" << "\n";
	std::cout << "     - 'sandboxed': base, math, string, table (no io/os)" << "\n";
	std::cout << "     - 'minimal': base only" << "\n";
	std::cout << "     - 'io': base, io, os" << "\n\n";

	ctx.CompileString("sandboxed_test", "print('  Using sandboxed pool (no io/os access)')");

	std::cout << "   Running on 'sandboxed' pool:" << "\n";
	try {
		ctx.RunPooled("sandboxed_test", "sandboxed");
	}
	catch (std::runtime_error& e) {
		std::cout << "   Error: " << e.what() << "\n";
	}
	std::cout << "\n";

	// ========================================
	// Example 5: Checking pool statistics
	// ========================================
	std::cout << "5. Checking pool statistics:" << "\n\n";

	StatePool& pool = ctx.getPool("default");
	std::cout << "   'default' pool statistics:" << "\n";
	std::cout << "     - Color: " << pool.getColor() << "\n";
	std::cout << "     - Max size: " << pool.getMaxSize() << "\n";
	std::cout << "     - Current size: " << pool.getCurrentSize() << "\n";
	std::cout << "     - Available: " << pool.availableCount() << "\n";
	std::cout << "     - Checked out: " << pool.checkedOutCount() << "\n";
	std::cout << "     - Thread safe: " << (pool.isThreadSafe() ? "yes" : "no") << "\n";
	std::cout << "\n";

	// ========================================
	// Example 6: Pool with global variables
	// ========================================
	std::cout << "6. Pool with global variables:" << "\n";
	std::cout << "   Configure a pool to automatically set global variables" << "\n";
	std::cout << "   in every state created from the pool." << "\n\n";

	PoolConfig configWithGlobals;
	configWithGlobals.maxSize = 2;
	configWithGlobals.AddGlobalVariable("app_version", std::make_shared<LuaTString>("1.0.0"));
	configWithGlobals.AddGlobalVariable("debug_mode", std::make_shared<LuaTBoolean>(false));

	ctx.createPool("with_globals", configWithGlobals);

	ctx.CompileString("check_globals", "print('  app_version = ' .. app_version .. ', debug_mode = ' .. tostring(debug_mode))");

	std::cout << "   Running on pool with predefined globals:" << "\n";
	try {
		ctx.RunPooled("check_globals", "with_globals");
	}
	catch (std::runtime_error& e) {
		std::cout << "   Error: " << e.what() << "\n";
	}
	std::cout << "\n";

	std::cout << "=== Example complete ===" << "\n";

	return 0;
}
