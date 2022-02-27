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
#include <memory>
#include <stdexcept>

#include "LuaCompiler.hpp"
#include "../Engine/LuaState.hpp"

using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;

void inline _checkErrorAndThrow(LuaState &L, int error) {
	if (error != LUA_OK) {
		switch (error) {
			case LUA_ERRMEM:
				throw std::runtime_error("Out of memory");
				break;
#if LUA_VERSION_NUM < 504
			case LUA_ERRGCMM:
				throw std::out_of_range("GC Error while loading");
				break;
#endif
			case LUA_ERRSYNTAX:
				throw std::logic_error(lua_tostring(L,1));
				break;
			default:
				throw std::runtime_error("Unknown error code "+ std::to_string(error) + " :" +lua_tostring(L,1));
		}
	}
}

std::unique_ptr<LuaCodeSnippet> LuaCompiler::CompileString(std::string name, std::string code) {
	std::unique_ptr<LuaCodeSnippet> cb_ptr = std::make_unique<LuaCodeSnippet>();

	LuaState L;
	int res = luaL_loadstring(L.getState(), code.c_str());
	_checkErrorAndThrow(L, res);

	res = lua_dump(L.getState(), code_writer, (void*) cb_ptr.get(), 0);
	_checkErrorAndThrow(L, res);

	cb_ptr->setName(name);
	return cb_ptr;
}

std::unique_ptr<LuaCodeSnippet> LuaCompiler::CompileFile(std::string name, std::string fname) {
	std::unique_ptr<LuaCodeSnippet> cb_ptr = std::make_unique<LuaCodeSnippet>();

	LuaState L;
	int res = luaL_loadfile(L, fname.c_str());
	_checkErrorAndThrow(L, res);
	
	res = lua_dump(L, code_writer, (void*) cb_ptr.get(), 0);
	_checkErrorAndThrow(L, res);

	cb_ptr->setName(name);
	return cb_ptr;
}

