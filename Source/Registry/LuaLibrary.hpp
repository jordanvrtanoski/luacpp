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

#ifndef LUACPP_LUALIBRARY_HPP
#define LUACPP_LUALIBRARY_HPP

#include <string>
#include <map>

#include "../Lua.hpp"
#include "LuaCFunction.hpp"
#include "../Engine/LuaState.hpp"

namespace LuaCpp {
	namespace Registry {
		
		/**
		 * @brief Wrapper of a lua library of `C` functions.
		 *
		 * @details
		 * Allows creating a library holding pointers to `C` functions
		 * that will be registerd under the library name in the 
		 * lua virtual machine. Each function has it's own unique name
		 * and should implement the `lua_CFunction` interface as specified
		 * in Lua documents.
		 *
		 */
		class LuaLibrary {
		   private:
			/**
			 * @brief map containing the function pointers and their names
			 */
			std::map<std::string, LuaCFunction> functions;

			/**
			 * @brief library name
			 */
			std::string name;

		   protected:
			/**
			 * @brief protected constructor
			 */
			LuaLibrary() {}

		   public:

			/**
			 * @brief construct new library with a given name
			 *
			 * @details
			 * Default constructor used to construct the library
			 * with a given name
			 */
			explicit LuaLibrary(const std::string &_name) : name(std::move(_name)) {}

			/**
			 * @bried default destrucor
			 *
			 * @details
			 * Default destructor
			 */
			~LuaLibrary() {}
			
			/**
			 * @brief get the library name
			 *
			 * @details
			 * Get the library name
			 *
			 * @return library name
			 */
			std::string getName();

			/**
			 * @brief set the name of the library
			 *
			 * @details
			 * Set the name of the library that will be used when registering 
			 * the functions in the LuaState
			 *
			 * @param name the library name
			 */
			void setName(const std::string &name);

			/**
			 * @brief Check if the function exists in the library
			 *
			 * @details
			 * Check if the library contains function under the given name
			 *
			 * @return true if the function name is already in the library, otherwise false
			 */
			 bool inline Exists(const std::string &name) {
				return !(functions.find( name ) == functions.end());
			}
			/**
			 * @brief Add function to the library
			 *
			 * @details
			 * Add a function to the library under the specified name.
			 * The function will be available in the lua virtual machine
			 * under the name `library_name.function_name`
			 * 
			 * @param name the name of the function in the lua context
			 * @param cfunction the function implementing lua_CFunction interface
			 *
			 */
			void AddCFunction(const std::string &name, lua_CFunction cfunction);
			
			/**
			 * @brief Add function to the library
			 *
			 * @details
			 * Add a function to the library under the specified name.
			 * The function will be available in the lua virtual machine
			 * under the name `library_name.function_name`
			 *
			 * @param name the name of the function in the lua context
			 * @param cfunction the function implementing lua_CFunction interface
			 * @param replace replaces the function if already exits
			 */
			void AddCFunction(const std::string &name, lua_CFunction cfunction, bool replace);

			/**
			 * @brief Register the library in the created state
			 *
			 * @details
			 * Registers the library and the functions in the provided LuaState
			 * under the name of the library.
			 *
			 * @return error codes from the lua machine
			 */
			int RegisterFunctions(Engine::LuaState &L);

		};
	}
}

/**
 * Example for the use of the LuaLibrary
 *
 * @example example_LuaLibrary.cpp
 */

#endif // LUACPP_LUALIBRARY_HPP
