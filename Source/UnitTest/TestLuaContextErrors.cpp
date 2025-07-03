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

namespace LuaCpp {

	class TestLuaContextErrors : public ::testing::Test {
	  protected:
		virtual void SetUp() {}
	};

	TEST_F(TestLuaContextErrors, CompileFile_NonExistent) {
		/**
		 * Test that compiling a non-existent file throws a runtime error.
		 */
		LuaContext ctx;
		EXPECT_THROW(ctx.CompileFile("test", "non_existent_file.lua"), std::runtime_error);
	}

	TEST_F(TestLuaContextErrors, Run_NonExistentSnippet) {
		/**
		 * Test that running a snippet that has not been compiled throws a runtime error.
		 */
		LuaContext ctx;
		EXPECT_THROW(ctx.Run("non_existent_snippet"), std::runtime_error);
	}

	TEST_F(TestLuaContextErrors, AddLibrary_NullPtr) {
		/**
		 * Test that adding a null library pointer throws an invalid_argument error.
		 */
		LuaContext ctx;
		EXPECT_THROW(ctx.AddLibrary(nullptr), std::invalid_argument);
	}

	TEST_F(TestLuaContextErrors, AddGlobalVariable_NullPtr) {
		/**
		 * Test that adding a null global variable pointer throws an invalid_argument error.
		 */
		LuaContext ctx;
		EXPECT_THROW(ctx.AddGlobalVariable("test_var", nullptr), std::invalid_argument);
	}

	TEST_F(TestLuaContextErrors, GetGlobalVariable_NonExistent) {
		/**
		 * Test that getting a non-existent global variable returns null.
		 */
		LuaContext ctx;
		EXPECT_EQ(nullptr, ctx.getGlobalVariable("non_existent_var"));
	}

	TEST_F(TestLuaContextErrors, GetStdLibrary_NonExistent) {
		/**
		 * Test that getting a non-standard library throws returns null.
		 */
		LuaContext ctx;
		EXPECT_EQ(nullptr, ctx.getStdLibrary("non_existent_lib"));
	}
}