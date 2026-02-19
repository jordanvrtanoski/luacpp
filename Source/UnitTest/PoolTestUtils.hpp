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

#ifndef LUACPP_POOLTESTUTILS_HPP
#define LUACPP_POOLTESTUTILS_HPP

#include "../Lua.hpp"
#include "gtest/gtest.h"

namespace LuaCpp {
namespace Test {

inline void ExpectGlobalIsTable(lua_State* L, const char* name) {
	lua_getglobal(L, name);
	EXPECT_TRUE(lua_istable(L, -1));
	lua_pop(L, 1);
}

inline void ExpectGlobalIsNil(lua_State* L, const char* name) {
	lua_getglobal(L, name);
	EXPECT_TRUE(lua_isnil(L, -1));
	lua_pop(L, 1);
}

inline void ExpectGlobalIsFunction(lua_State* L, const char* name) {
	lua_getglobal(L, name);
	EXPECT_TRUE(lua_isfunction(L, -1));
	lua_pop(L, 1);
}

inline void ExpectGlobalIsNumber(lua_State* L, const char* name, lua_Number expected) {
	lua_getglobal(L, name);
	EXPECT_TRUE(lua_isnumber(L, -1));
	EXPECT_DOUBLE_EQ(expected, lua_tonumber(L, -1));
	lua_pop(L, 1);
}

inline void ExpectAllStandardLibraries(lua_State* L) {
	ExpectGlobalIsTable(L, "io");
	ExpectGlobalIsTable(L, "math");
	ExpectGlobalIsTable(L, "os");
	ExpectGlobalIsTable(L, "string");
	ExpectGlobalIsTable(L, "table");
	ExpectGlobalIsTable(L, "debug");
	ExpectGlobalIsTable(L, "coroutine");
	ExpectGlobalIsTable(L, "package");
	ExpectGlobalIsTable(L, "utf8");
}

inline void ExpectBaseLibrariesOnly(lua_State* L) {
	ExpectGlobalIsFunction(L, "print");
	ExpectGlobalIsNil(L, "math");
	ExpectGlobalIsNil(L, "io");
}

inline void ExpectSandboxedLibraries(lua_State* L) {
	ExpectGlobalIsTable(L, "math");
	ExpectGlobalIsTable(L, "string");
	ExpectGlobalIsNil(L, "io");
	ExpectGlobalIsNil(L, "os");
}

inline void ExpectIOLibraries(lua_State* L) {
	ExpectGlobalIsTable(L, "io");
	ExpectGlobalIsTable(L, "os");
	ExpectGlobalIsNil(L, "math");
}

inline void ExpectDefaultLibraries(lua_State* L) {
	ExpectGlobalIsTable(L, "io");
	ExpectGlobalIsTable(L, "math");
}

}
}

#endif // LUACPP_POOLTESTUTILS_HPP
