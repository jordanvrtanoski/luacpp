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

#include "LuaTBoolean.hpp"

using namespace LuaCpp::Engine;

int LuaTBoolean::getTypeId() const {
	return LUA_TBOOLEAN;
}

std::string LuaTBoolean::getTypeName(LuaState &L) const {
	return std::string(lua_typename(L, LUA_TBOOLEAN));
}

void LuaTBoolean::PushValue(LuaState &L) {
	lua_pushboolean(L, value);
}

void LuaTBoolean::PopValue(LuaState &L, int idx) {
	if (lua_type(L, idx) == LUA_TBOOLEAN) {
		value = lua_toboolean(L,idx);
	} else {
		throw std::invalid_argument("The value at the stack position " + std::to_string(idx) + " is not LUA_TBOOLEAN");
	}
}

std::string LuaTBoolean::ToString() const {
	if (value) {
		return "true";
	} else {
		return "false";
	}
}

bool LuaTBoolean::getValue() const {
	return value;
}

void LuaTBoolean::setValue(bool _value) {
	value = _value;
}

