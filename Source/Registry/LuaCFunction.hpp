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

#ifndef LUACPP_LUACFUNCTION_HPP
#define LUACPP_LUACFUNCTION_HPP

#include <string>

#include "../Lua.hpp"

namespace LuaCpp {
	namespace Registry {

		/**
		 * @brief Wrapper for a `C` function implementing the lua_CFunction interface
		 *
		 * @details
		 * Wrapper for the lua_CFunction that will be added to the LuaLibrary.
		 */
		class LuaCFunction {
		   private:
			std::string name;
			lua_CFunction cfunction;

	           public:
			LuaCFunction() {};
			explicit LuaCFunction(lua_CFunction _cfunction) : cfunction(_cfunction) {};
			~LuaCFunction() {};

			void setName(std::string name);
			std::string getName();
		
			void setCFunction(lua_CFunction cfunction);
			lua_CFunction getCFunction();
		};
	}
}

#endif // LUACPP_LUACFUNCTION_HPP
