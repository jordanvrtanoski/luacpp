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

      // Used for testing of the C library bridge
      // return only a constant
      static int bar (lua_State *L) {
        lua_Number sum = 4.2;
        lua_pushnumber(L, sum);        /* first result */
        return 1;                   /* number of results */
      }
}

namespace LuaCpp {

	class TestLuaContext : public ::testing::Test {
	  protected:
		virtual void SetUp() {
			std::ofstream of("TestLuaContext_1_ok.lua", std::ofstream::out | std::ofstream::trunc );
			of << "print(\"Hello World from Lua\")";
			of.close();

			of.open("TestLuaContext_2_nok.lua", std::ofstream::out | std::ofstream::trunc );
			of << "print_not_exists(\"Hello World from Lua\")";
			of.close();

			of.open("TestLuaContext_3_v1.lua", std::ofstream::out | std::ofstream::trunc );
			of << "print('Hello World from Lua, v1.0')";
			of.close();

			of.open("TestLuaContext_3_v2.lua", std::ofstream::out | std::ofstream::trunc );
			of << "print('Hello World from Lua, v2.0')";
			of.close();
			
			of.open("TestLuaContext_3_v3.lua", std::ofstream::out | std::ofstream::trunc );
			of << "print('Hello World from Lua, v3.0')";
			of.close();

			of.open("TestLuaContext_4_se.lua", std::ofstream::out | std::ofstream::trunc );
			of << "while {}[1]";
			of.close();
		}
	};

	TEST_F(TestLuaContext, GetValidLuaContext) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<Engine::LuaState> L = ctx.newState();

		EXPECT_NE((Engine::LuaState *) NULL, L.get());
		EXPECT_EQ(0, lua_gettop(*L));
		EXPECT_NE((const char *) NULL, lua_pushstring(*L, "test"));
		EXPECT_EQ(1, lua_gettop(*L));
	}

	TEST_F(TestLuaContext, NonExistingStateThrowsException) {
		/**
		 * The code is invalid and should throw an compilation error
		 */
		LuaContext ctx;

		EXPECT_THROW(ctx.newStateFor("test"), std::runtime_error) ;
	}

	TEST_F(TestLuaContext, ReuseLuaSteteFromContext) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<Engine::LuaState> L = ctx.newState();

		EXPECT_NE((Engine::LuaState *) NULL, L.get());
		EXPECT_EQ(0, lua_gettop(*L));
		EXPECT_NE((const char *) NULL, lua_pushstring(*L, "test"));
		EXPECT_EQ(1, lua_gettop(*L));

		// Reuse the state
		std::unique_ptr<Engine::LuaState> Ln = std::make_unique<Engine::LuaState>(*L, true);
		EXPECT_EQ(1, lua_gettop(*Ln));
		Ln.reset();
		
		// The state should not be closed
		EXPECT_EQ(1, lua_gettop(*L));
	}

	TEST_F(TestLuaContext, CompileError) {
		/**
		 * The code is invalid and should throw an compilation error
		 */
		LuaContext ctx;

		EXPECT_THROW(ctx.CompileString("test", "while {}[1]"), std::logic_error) ;
	}

	TEST_F(TestLuaContext, CompileFolderWithPrefixNoRecompile) {
		LuaContext ctx;
			
		EXPECT_NO_THROW(ctx.CompileFolder("./", "local", false));

		EXPECT_NO_THROW(ctx.newStateFor("local.TestLuaContext_3_v3"));

		EXPECT_THROW(ctx.newStateFor("local.TestLuaContext_4_se"), std::runtime_error);
	}

	TEST_F(TestLuaContext, CompileFolderNoPrefixNoRecompile) {
		LuaContext ctx;
			
		EXPECT_NO_THROW(ctx.CompileFolder("./", "", false));

		EXPECT_NO_THROW(ctx.newStateFor("TestLuaContext_3_v3"));

		EXPECT_THROW(ctx.newStateFor("TestLuaContext_4_se"), std::runtime_error);
	}

	TEST_F(TestLuaContext, CompileFolderWithPrefix) {
		LuaContext ctx;
			
		EXPECT_NO_THROW(ctx.CompileFolder("./", "local"));

		EXPECT_NO_THROW(ctx.newStateFor("local.TestLuaContext_3_v3"));

		EXPECT_THROW(ctx.newStateFor("local.TestLuaContext_4_se"), std::runtime_error);
	}

	TEST_F(TestLuaContext, CompileFolder) {
		LuaContext ctx;
			
		EXPECT_NO_THROW(ctx.CompileFolder("./"));

		EXPECT_NO_THROW(ctx.newStateFor("TestLuaContext_3_v3"));

		EXPECT_THROW(ctx.newStateFor("TestLuaContext_4_se"), std::runtime_error);
	}



	TEST_F(TestLuaContext, HelloWorldFromLuaString) {
		/**
		 * The snippet should print "Hello World from Lua" message on the screen.
		 */
		LuaContext ctx;

		testing::internal::CaptureStdout();

		EXPECT_NO_THROW(ctx.CompileStringAndRun("print('Hello World from Lua')"));

		std::string output = testing::internal::GetCapturedStdout();

		EXPECT_EQ("Hello World from Lua\n", output);

	}

	TEST_F(TestLuaContext, CheckLuaCppVersionInLuaContext) {
		/**
		 * The snippet should print the version message on the screen.
		 */
		LuaContext ctx;

		testing::internal::CaptureStdout();

		EXPECT_NO_THROW(ctx.CompileStringAndRun("print(_luacppversion)"));

		std::string output = testing::internal::GetCapturedStdout();

		EXPECT_EQ(std::string(LuaCpp::Version)+"\n", output);

	}


	TEST_F(TestLuaContext, RuntimeErrorFromLuaString) {
		/**
		 * The code is syntacticaly correct, but contains call to unexiting function.
		 * This should generate runtime_error
		 */
		LuaContext ctx;

		EXPECT_THROW(ctx.CompileStringAndRun("print_not_found('Hello World from Lua')"), std::runtime_error);

	}

	TEST_F(TestLuaContext, DiscardRecompiledSnipet) {
		/**
		 * Only the first version of the snippet is expected to
		 * remain in the repository. The subsequent versions are
		 * discarded in the CompileString method.
		 */
		LuaContext ctx;

		testing::internal::CaptureStdout();

		EXPECT_NO_THROW(ctx.CompileString("test", "print('Hello World from Lua, v1.0')"));
		EXPECT_NE((Engine::LuaState *) NULL, ctx.newStateFor("test").get());

		EXPECT_NO_THROW(ctx.CompileString("test", "print('Hello World from Lua, v2.0')"));
		EXPECT_NE((Engine::LuaState *) NULL, ctx.newStateFor("test").get());
	
		EXPECT_NO_THROW(ctx.CompileString("test", "print('Hello World from Lua, v3.0')"));
		EXPECT_NE((Engine::LuaState *) NULL, ctx.newStateFor("test").get());
		
		EXPECT_NO_THROW(ctx.Run("test"));
		
		std::string output = testing::internal::GetCapturedStdout();
	
		EXPECT_EQ("Hello World from Lua, v1.0\n", output);

	}

	TEST_F(TestLuaContext, AllowRecompiledSnipet) {
		/**
		 * Only the first version of the snippet is expected to
		 * remain in the repository. The subsequent versions are
		 * discarded in the CompileString method.
		 */
		LuaContext ctx;

		testing::internal::CaptureStdout();

		EXPECT_NO_THROW(ctx.CompileString("test", "print('Hello World from Lua, v1.0')", true));
		EXPECT_NE((Engine::LuaState *) NULL, ctx.newStateFor("test").get());

		EXPECT_NO_THROW(ctx.CompileString("test", "print('Hello World from Lua, v2.0')", true));
		EXPECT_NE((Engine::LuaState *) NULL, ctx.newStateFor("test").get());
	
		EXPECT_NO_THROW(ctx.CompileString("test", "print('Hello World from Lua, v3.0')", true));
		EXPECT_NE((Engine::LuaState *) NULL, ctx.newStateFor("test").get());
		
		EXPECT_NO_THROW(ctx.Run("test"));
		
		std::string output = testing::internal::GetCapturedStdout();
	
		EXPECT_EQ("Hello World from Lua, v3.0\n", output);

	}

	TEST_F(TestLuaContext, HelloWorldFromLuaFile) {
		/**
		 * The snippet should print "Hello World from Lua" message on the screen.
		 */
		LuaContext ctx;

		testing::internal::CaptureStdout();

		EXPECT_NO_THROW(ctx.CompileFileAndRun("TestLuaContext_1_ok.lua"));

		std::string output = testing::internal::GetCapturedStdout();

		EXPECT_EQ("Hello World from Lua\n", output);

	}
	
	TEST_F(TestLuaContext, RuntimeErrorFromLuaFile) {
		/**
		 * The code is syntacticaly correct, but contains call to unexiting function.
		 * This should generate runtime_error
		 */
		LuaContext ctx;

		EXPECT_THROW(ctx.CompileFileAndRun("TestLuaContext_2_nok.lua"), std::runtime_error);

	}

	TEST_F(TestLuaContext, DiscardRecompiledSnipetFile) {
		/**
		 * Only the first version of the snippet is expected to
		 * remain in the repository. The subsequent versions are
		 * discarded in the CompileString method.
		 */
		LuaContext ctx;

		testing::internal::CaptureStdout();

		EXPECT_NO_THROW(ctx.CompileFile("test", "TestLuaContext_3_v1.lua"));
		EXPECT_NE((Engine::LuaState *) NULL, ctx.newStateFor("test").get());

		EXPECT_NO_THROW(ctx.CompileFile("test", "TestLuaContext_3_v2.lua"));
		EXPECT_NE((Engine::LuaState *) NULL, ctx.newStateFor("test").get());
	
		EXPECT_NO_THROW(ctx.CompileFile("test", "TestLuaContext_3_v3.lua"));
		EXPECT_NE((Engine::LuaState *) NULL, ctx.newStateFor("test").get());
		
		EXPECT_NO_THROW(ctx.Run("test"));
		
		std::string output = testing::internal::GetCapturedStdout();
	
		EXPECT_EQ("Hello World from Lua, v1.0\n", output);

	}
	
	TEST_F(TestLuaContext, AllowRecompiledSnipetFile) {
		/**
		 * Only the first version of the snippet is expected to
		 * remain in the repository. The subsequent versions are
		 * discarded in the CompileString method.
		 */
		LuaContext ctx;

		testing::internal::CaptureStdout();

		EXPECT_NO_THROW(ctx.CompileFile("test", "TestLuaContext_3_v1.lua", true));
		EXPECT_NE((Engine::LuaState *) NULL, ctx.newStateFor("test").get());

		EXPECT_NO_THROW(ctx.CompileFile("test", "TestLuaContext_3_v2.lua", true));
		EXPECT_NE((Engine::LuaState *) NULL, ctx.newStateFor("test").get());
	
		EXPECT_NO_THROW(ctx.CompileFile("test", "TestLuaContext_3_v3.lua", true));
		EXPECT_NE((Engine::LuaState *) NULL, ctx.newStateFor("test").get());
		
		EXPECT_NO_THROW(ctx.Run("test"));
		
		std::string output = testing::internal::GetCapturedStdout();
	
		EXPECT_EQ("Hello World from Lua, v3.0\n", output);

	}

	TEST_F(TestLuaContext, RegisterCLibrary) {
		/**
		 * Register the "foo" function written in "C" as a library
		 * and execute it in the new State.
		 */
		LuaContext ctx;

		std::shared_ptr<Registry::LuaLibrary> lib = std::make_shared<Registry::LuaLibrary>("foolib");
		lib->AddCFunction("foo", foo);

		testing::internal::CaptureStdout();

		EXPECT_NO_THROW(ctx.AddLibrary(lib));
        EXPECT_NO_THROW(ctx.CompileString("test", "print(\"Result of calling foolib.foo(1,2,3,4) = \" .. foolib.foo(1,2,3,4))"));
		
		EXPECT_NO_THROW(ctx.Run("test"));
		
		std::string output = testing::internal::GetCapturedStdout();
	
		EXPECT_EQ("Result of calling foolib.foo(1,2,3,4) = 2.5\n", output);

	}

    TEST_F(TestLuaContext, RegisterCLibrary2Functions) {
        /**
         * Register the "foo" and "bar" function written in "C" as a library
         * and execute it in the new State.
         */
        LuaContext ctx;

        std::shared_ptr<Registry::LuaLibrary> lib = std::make_shared<Registry::LuaLibrary>("foolib");
        lib->AddCFunction("foo", foo);
        lib->AddCFunction("bar", bar);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(ctx.AddLibrary(lib));
        EXPECT_NO_THROW(ctx.CompileString("test", "print(\"multiple functions\", foolib.foo(1,2,3,4), foolib.bar())"));

        EXPECT_NO_THROW(ctx.Run("test"));

        std::string output = testing::internal::GetCapturedStdout();

        EXPECT_EQ("multiple functions\t2.5\t4.2\n", output);

    }

	TEST_F(TestLuaContext, RegisterCLibraryWithChangedName) {
		/**
		 * Register the "foo" function written in "C" as a library
		 * and execute it in the new State.
		 */
		LuaContext ctx;

		std::shared_ptr<Registry::LuaLibrary> lib = std::make_shared<Registry::LuaLibrary>("some_foolib");
		lib->AddCFunction("foo", foo);
		lib->setName("foolib");

		testing::internal::CaptureStdout();

		EXPECT_NO_THROW(ctx.AddLibrary(lib));
		EXPECT_NO_THROW(ctx.CompileString("test", "print(\"Result of calling foolib.foo(1,2,3,4) = \" .. foolib.foo(1,2,3,4))"));
		
		EXPECT_NO_THROW(ctx.Run("test"));
		
		std::string output = testing::internal::GetCapturedStdout();
	
		EXPECT_EQ("Result of calling foolib.foo(1,2,3,4) = 2.5\n", output);

	}



	TEST_F(TestLuaContext, TestGlobalVariables) {
		LuaContext ctx;

		std::shared_ptr<Engine::LuaTString> str = std::make_shared<Engine::LuaTString>("testing 1,2,3");
		testing::internal::CaptureStdout();

		EXPECT_NO_THROW(ctx.AddGlobalVariable("test_str", str));
		EXPECT_NO_THROW(ctx.CompileString("test", "print(test_str) test_str = 'changed'"));
		
		EXPECT_NO_THROW(ctx.Run("test"));
		
		std::string output = testing::internal::GetCapturedStdout();
	
		EXPECT_EQ("testing 1,2,3\n", output);
		EXPECT_EQ("changed", str->getValue());
		EXPECT_NE((void *) NULL, (void *) &ctx.getGlobalVariable("test_str"));
		std::shared_ptr<Engine::LuaTString> str2 = std::static_pointer_cast<Engine::LuaTString>(ctx.getGlobalVariable("test_str"));
		EXPECT_EQ("changed", str2->getValue());


	}

	TEST_F(TestLuaContext, TestEnvironmentVariables) {
		LuaContext ctx;

		std::shared_ptr<Engine::LuaTString> str = std::make_shared<Engine::LuaTString>("testing 1,2,3");
		LuaEnvironment env;

		env["test_str"] = str;

		testing::internal::CaptureStdout();

		EXPECT_NO_THROW(ctx.CompileString("test", "print(test_str) test_str = 'changed'"));
		
		EXPECT_NO_THROW(ctx.RunWithEnvironment("test", env));
		
		std::string output = testing::internal::GetCapturedStdout();
	
		EXPECT_EQ("testing 1,2,3\n", output);
		EXPECT_EQ("changed", str->getValue());
		EXPECT_EQ((void *) NULL, (void *) &(*ctx.getGlobalVariable("test_str")));

	}

}
