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

#include <iostream>

#include "LuaTUserData.hpp"

using namespace LuaCpp::Engine;

int LuaTUserData::getTypeId() const {
	return LUA_TUSERDATA;
}

std::string LuaTUserData::getTypeName(LuaState &L) const {
	return std::string(lua_typename(L, LUA_TUSERDATA));
}

void LuaTUserData::PushValue(LuaState &L) {
	userdata = lua_newuserdata(L, size);
	if (metatable.size() > 0) {
		lua_newtable(L);
		for (const auto &pairs: metatable) {
			lua_pushcfunction(L, pairs.second);
			lua_setfield(L, -2, pairs.first.c_str());
		}
		lua_setmetatable(L, -2);
	}
	_storeData();
}

void LuaTUserData::PopValue(LuaState &L, int idx) {
	if (lua_type(L, idx) == LUA_TUSERDATA) {
		void * _userdata = lua_touserdata(L, idx);
		if (_userdata == userdata) {
			_retreiveData();
		} else {
			throw std::domain_error("The value on the stack "+std::to_string(idx)+" has different pointer to  the userdata buffer.");
		}
	} else {
		throw std::invalid_argument("The value at the stack position " + std::to_string(idx) + " is not LUA_TNUMBER");
	}
}

std::string LuaTUserData::ToString() const {
	return "userdata";
}

void LuaTUserData::_storeData() {
}

void LuaTUserData::_retreiveData() {
}

void* LuaTUserData::getRawUserData() const {
	return userdata;
}

void LuaTUserData::AddMetaFunction(std::string fname, lua_CFunction fn) {
	metatable[fname] = fn;
}
