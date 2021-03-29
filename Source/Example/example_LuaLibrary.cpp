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

extern "C" {
   static int _foo (lua_State *L, int start) {
     int n = lua_gettop(L);    /* number of arguments */
     lua_Number sum = 0.0;
     int i;
     for (i = start; i <= n; i++) {
       if (!lua_isnumber(L, i)) {
         lua_pushliteral(L, "incorrect argument");
         lua_error(L);
       }
       sum += lua_tonumber(L, i);
     }
     lua_pushnumber(L, sum/n);        /* first result */
     lua_pushnumber(L, sum);         /* second result */
     return 2;                   /* number of results */
   }
   
   static int foo(lua_State *L) {
     return _foo(L, 1);
   }
   
   static int foo_meta (lua_State *L) {
     return _foo(L, 2);
   }
}

int main(int argc, char **argv) {

	// Creage Lua context
	LuaContext lua;

	// Create library "foo" conatining the "foo" function
	std::shared_ptr<LuaLibrary> lib = std::make_shared<LuaLibrary>("foolib");
	lib->AddCFunction("foo", foo);

	// Add library to the context
	lua.AddLibrary(lib);

	// Compile a code using the new foolib.foo function
	lua.CompileString("foo_test", "print(\"Result of calling foolib.foo(1,2,3,4) = \" .. foolib.foo(1,2,3,4))");

	// Run the context	
	try {
		lua.Run("foo_test");
	}
	catch (std::runtime_error& e)
  	{
		std::cout << e.what() << '\n';
  	}

	lua.CompileString("test", "print('Calling foo as a metafunction of a usertype ' .. foo(1,2,3,4))");
	std::unique_ptr<LuaState> L = lua.newStateFor("test");

	LuaTUserData ud(sizeof(LuaTUserData *));
	
	ud.AddMetaFunction("__call", foo_meta);
	
	ud.PushGlobal(*L, "foo");

	int res = lua_pcall(*L, 0, LUA_MULTRET, 0);
	if (res != LUA_OK ) {
		std::cout << "Error Executing " << res << " " << lua_tostring(*L,1) << "\n";	
	}


}
