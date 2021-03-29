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

#include "LuaMetaObject.hpp"

extern "C" {
	static int u_newindex(lua_State *L);
	static int u_index(lua_State *L);
	static int u_call(lua_State *L);
}

using namespace LuaCpp;
using namespace LuaCpp::Engine;

LuaMetaObject::LuaMetaObject() : LuaTUserData(sizeof(LuaMetaObject *)) {
	AddMetaFunction("__index", u_index);
	AddMetaFunction("__newindex", u_newindex);
	AddMetaFunction("__call", u_call);
}

void LuaMetaObject::_storeData() {
	*((void **) userdata) = (void *) this;
}

void LuaMetaObject::_retreiveData() {
}

int LuaMetaObject::getValue(LuaState &L) {
	if (lua_type(L, 2) == LUA_TSTRING) {
		std::cout << "Calling getValue() with "<< lua_tostring(L, 2) << "\n";
	} else {
		std::cout << "Calling getValue() with "<< std::to_string(lua_tointeger(L, 2)) << "\n";
	}
	lua_pushstring(L, "testing");
	return 1;
}


int LuaMetaObject::setValue(LuaState &L) {
	std::cout << "setValue() \n";
	L.PrintStack(std::cout);
	return 0;
}

int LuaMetaObject::Execute(LuaState &L) {
	std::cout << "Execute() \n";
	L.PrintStack(std::cout);
	return 0;
}

static int u_newindex(lua_State *L) {
	void * ud = lua_touserdata(L, 1);
	LuaState _L(L, true);
	int res = (**((LuaMetaObject **) ud)).setValue(_L);

	return res;
}

static int u_index(lua_State *L) {
	void * ud = lua_touserdata(L, 1);
	LuaState _L(L, true);
	int res = (**((LuaMetaObject **) ud)).getValue(_L);

	return res;
}


static int u_call(lua_State *L) {
	void * ud = lua_touserdata(L, 1);
	LuaState _L(L, true);
	int res = (**((LuaMetaObject **) ud)).Execute(_L);

	return res;
}
