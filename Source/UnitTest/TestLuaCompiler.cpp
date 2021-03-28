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

#include <fstream>

#include "../LuaCpp.hpp"
#include "gtest/gtest.h"


extern "C" {
	// Used for testing of the C library bridge
     static int foo (lua_State *L) {
       int n = lua_gettop(L);    /* number of arguments */
       lua_Number sum = 0.0;
       int i;
       for (i = 1; i <= n; i++) {
/*
         if (!lua_isnumber(L, i)) {
           lua_pushliteral(L, "incorrect argument");
           lua_error(L);
         }
*/
         sum += lua_tonumber(L, i);
       }
       lua_pushnumber(L, sum/n);        /* first result */
       lua_pushnumber(L, sum);         /* second result */
       return 2;                   /* number of results */
     }
}

using namespace LuaCpp::Engine;
using namespace LuaCpp::Registry;

namespace LuaCpp {

	class TestLuaCompiler : public ::testing::Test {
	  protected:
		virtual void SetUp() {
			/*
			std::ofstream of("TestLuaCompiler_1_ok.lua", std::ofstream::out | std::ofstream::trunc );
			of << "print(\"Hello World from Lua\")";
			of.close();
			*/
		}
	};

	TEST_F(TestLuaCompiler, TestLibrary) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<Engine::LuaState> L = ctx.newState();

		EXPECT_NE((Engine::LuaState *) NULL, L.get());
		EXPECT_EQ(0, lua_gettop(*L));
		EXPECT_NE((const char *) NULL, lua_pushstring(*L, "test"));
		EXPECT_EQ(1, lua_gettop(*L));


		std::unique_ptr<LuaLibrary> lib = std::make_unique<LuaLibrary>("foolib");
		lib->AddCFunction("foo", foo);

		// Add library to the context
		EXPECT_NO_THROW(ctx.AddLibrary(std::move(lib)));

		// Compile a code using the new foolib.foo function
		EXPECT_NO_THROW(ctx.CompileString("foo_test", "print(\"Result of calling foolib.foo(1,2,3,4) = \" .. foolib.foo(1,2,3,4))"));

		EXPECT_NO_THROW(ctx.Run("foo_test"));
	}

	TEST_F(TestLuaCompiler, TestCFunction) {
		LuaCFunction cf;

		EXPECT_NO_THROW(cf.setName("test"));
		EXPECT_EQ("test", cf.getName());
		
		EXPECT_NO_THROW(cf.setCFunction(foo));
	}

}


