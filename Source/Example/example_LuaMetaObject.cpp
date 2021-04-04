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


class MetaMap : public LuaMetaObject {
	public:
	        std::map<std::string, std::shared_ptr<LuaType>> values;
		MetaMap() : values() {}
		bool inline Exists(const std::string &name) {
			return !(values.find( name ) == values.end());
		}

		std::shared_ptr<LuaType> getValue(std::string &key) {
			if (Exists(key)) {
				return values[key];
			}
			return std::make_shared<LuaTNil>();
		}

		void setValue(std::string &key, std::shared_ptr<LuaType> val) {
			values[key] = val;
		}

};


int main(int argc, char **argv) {

	// Creage Lua context
	LuaContext lua;

	lua.CompileString("test1", "print('Assigning to foo[\"1\"] value \"testing MetaMap\"') foo[\"1\"] = 'testing MetaMap'");
	lua.CompileString("test2", "print('foo[\"1\"] : ' .. foo[\"1\"] )");
	
	std::unique_ptr<LuaState> L = lua.newStateFor("test1");

	MetaMap obj;
	
	obj.PushGlobal(*L, "foo");

	int res = lua_pcall(*L, 0, LUA_MULTRET, 0);
	if (res != LUA_OK ) {
		std::cout << "Error Executing " << res << " " << lua_tostring(*L,1) << "\n";	
	}

	L = lua.newStateFor("test2");
	obj.PushGlobal(*L, "foo");

	res = lua_pcall(*L, 0, LUA_MULTRET, 0);
	if (res != LUA_OK ) {
		std::cout << "Error Executing " << res << " " << lua_tostring(*L,1) << "\n";	
	}

}
