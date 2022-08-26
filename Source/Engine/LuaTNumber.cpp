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

#include "LuaTNumber.hpp"

using namespace LuaCpp::Engine;

int LuaTNumber::getTypeId() const {
	return LUA_TNUMBER;
}

std::string LuaTNumber::getTypeName(LuaState &L) const {
	return std::string(lua_typename(L, LUA_TNUMBER));
}

void LuaTNumber::PushValue(LuaState &L) {
	lua_pushnumber(L, value);
}

void LuaTNumber::PopValue(LuaState &L, int idx) {
	if (lua_type(L, idx) == LUA_TNUMBER) {
		value = lua_tonumber(L,idx);
	} else {
		throw std::invalid_argument("The value at the stack position " + std::to_string(idx) + " is not LUA_TNUMBER");
	}
}

std::string LuaTNumber::ToString() const {
	return std::to_string(value);
}

double LuaTNumber::getValue() const {
	return value;
}

void LuaTNumber::setValue(double _value) {
	value = _value;
}
