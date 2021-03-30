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
#include <iostream>
#include <stdexcept>

using namespace LuaCpp;
using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;

int main(int argc, char **argv) {

	std::cout << "Hi from C++, this is a demo how LuaCpp can be used" << "\n";
	LuaContext lua;

	// The simples way is to use CompileStringAndRun method
	try {
		lua.CompileStringAndRun("print('The fastest way to start using lua in a project')");
	}
	catch (std::runtime_error& e)
  	{
		std::cout << e.what() << '\n';
  	}

	// Compie the file before use
	lua.CompileFile("hello", "hello.lua");

	// Running the snipped with high level methods
	try {
		lua.Run("hello");
	}
	catch (std::runtime_error& e)
  	{
		std::cout << e.what() << '\n';
  	}

	// Running the nsipped with low level methods and lua functions
	lua.CompileString("test", "test_tbl[\"arr\"] = {1,2,3,4} print('This snippet is used to demonstrate low level Lua calls from '..test_str ..' '..test_num .. ' ' .. tostring(test_bool)) test_bool=false test_str='hi C++ from Lua' print('The value of test_tbl[t2] = '..test_tbl[\"t2\"]) test_tbl[\"t3\"] = { name = 'lua'} test_num=4.3");
	std::unique_ptr<LuaState> L = lua.newStateFor("test");

	LuaTString str(" C++ wrapped string");
	str.PushGlobal(*L,"test_str");

	LuaTNumber num(5.1);
	num.PushGlobal(*L, "test_num");

	LuaTBoolean bol(false);
	bol.PushGlobal(*L, "test_bool");

	LuaTTable tbl;
	tbl.setValue(Table::Key("t1"), std::make_shared<LuaTString>("Test 1"));
	tbl.setValue(Table::Key("t2"), std::make_shared<LuaTString>("Test 2"));
	tbl.setValue(Table::Key("t4"), std::make_shared<LuaTBoolean>("false"));
	tbl.setValue(Table::Key("t5"), std::make_shared<LuaTNumber>(12.2));
	tbl.setValue(Table::Key(1), std::make_shared<LuaTNumber>(1));

	tbl.PushGlobal(*L, "test_tbl");

	
	int res = lua_pcall(*L, 0, LUA_MULTRET, 0);
	if (res != LUA_OK ) {
		std::cout << "Error Executing " << res << " " << lua_tostring(*L,1) << "\n";	
	}

	str.PopGlobal(*L);
	tbl.PopGlobal(*L);
	num.PopGlobal(*L);
	bol.PopGlobal(*L);
	std::cout << "Value from lua: "  << str.ToString() << ", " << num.ToString() << ", " << bol.ToString() << "\n";

	std::cout << "Table: " << tbl.ToString() << "\n";
}
