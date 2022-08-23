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

#include "LuaType.hpp"

using namespace LuaCpp::Engine;

LuaType::LuaType() : globalName() {
	global = false;
}

LuaType::~LuaType() {
}

void LuaType::PopValue(LuaState &L) {
	PopValue(L, -1);
}

void LuaType::PushGlobal(LuaState &L, std::string _globalName) {
	PushValue(L);
	globalName = std::move(_globalName);
	global = true;

	lua_setglobal(L, globalName.c_str());
}

void LuaType::PopGlobal(LuaState &L) {
	if (global) {
		lua_getglobal(L, globalName.c_str());
		PopValue(L);
		lua_pop(L,1);
	}
}

void LuaType::PopGlobal(LuaState &L, std::string _global_name) {
	global = true;
	globalName = std::move(_global_name);
	PopGlobal(L);
}

bool LuaType::isGlobal() const {
	return global;
}

std::string LuaType::getGlobalName() const {
	return globalName;
}
