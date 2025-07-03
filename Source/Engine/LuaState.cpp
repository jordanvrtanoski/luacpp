
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

#include "LuaState.hpp"

using namespace LuaCpp::Engine;

LuaState::LuaState() {
	shared = false;
	L = luaL_newstate();
}

LuaState::LuaState(StateParams params) {
   shared = false;
   L = lua_newstate(params.allocator, params.userData);
}

LuaState::~LuaState() {
	if (!shared) {
		lua_close(L);
	}
}

LuaState::operator lua_State *() {
	return getState();
}

lua_State * LuaState::getState() {
	return L;
}

void LuaState::PrintStack(std::ostream &out) {

	int top = lua_gettop(L);
	out << "Total on stack " << top << "\n";
	for (int i=1; i <= top; i++ ) {

		out << "[" << i << "] -> (" << lua_typename(L, lua_type(L, i)) << ") ";
	        if (lua_type(L,i) == LUA_TSTRING) {
			out << lua_tostring(L, i);
		}
		out <<  "\n";
	}
}
