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

#include "LuaLibrary.hpp"

using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;

std::string LuaLibrary::getName() {
	return name;
}

void LuaLibrary::setName(const std::string &_name) {
	name = std::move(_name);
}

void LuaLibrary::AddCFunction(const std::string &name, lua_CFunction cfunction) {
	AddCFunction(name, cfunction, false);	
}

void LuaLibrary::AddCFunction(const std::string &name, lua_CFunction cfunction, bool replace) {
	if (!Exists(name) or replace) {
		std::unique_ptr<LuaCFunction> func = std::make_unique<LuaCFunction>(cfunction);
		func->setName(name);
		functions.insert(std::make_pair(name, std::move(*func)));
	}
}

int LuaLibrary::RegisterFunctions(LuaState &L) {

	luaL_Reg reg[functions.size()+1];
	int count = 0;

	for (auto & x : functions) {
		LuaCFunction& lcf = x.second;
		 reg[count].name = x.first.c_str();
		reg[count].func = lcf.getCFunction();
		count++;
	}
	
	reg[count].name = NULL;
	reg[count].func = NULL;

	luaL_newlib(L, reg);
	lua_setglobal(L,name.c_str());

	return 0;
}
