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

#ifndef LUACPP_LUARGISTRY_HPP
#define LUACPP_LUARGISTRY_HPP

#include <string>
#include <map>
#include <memory>

#include "../Lua.hpp"
#include "LuaCodeSnippet.hpp"

namespace LuaCpp {
	namespace Registry {
		
		/**
		 * @brief Registry containing the code snippets and customer libraries
		 *
		 * @details
		 * The registry belongs to the LuaContext and holds the references to the
		 * custom `C/C++` libraries and the code snippets. 
		 */
		class LuaRegistry {
		   private:
			/**
			 * @brief Map containing the code snippets
			 *
			 * @details
			 * Map containing the code snippets. The key of the map is the 
			 * name of the snippet under which it's registered in the 
			 * registry.
			 */
			std::map<std::string, LuaCodeSnippet> registry;
		   public:
			LuaRegistry() : registry() {};
			~LuaRegistry() {} ; 

			/**
			 * @brief Compiles a string and adds it to the registry
			 *
			 * @details 
			 * Createas an instance of the LuaCompiler and uses it to compile
			 * the code. If the code is compiled sucessfully, it will be
			 * added to the registry under the provided name.
			 * 
			 * If the code snippet under this name already exists in the repository, 
			 * the method will ignore the new code provided in the call.
			 *
			 * @see CompileAndAddString(std::string name, std::string code, bool recompile)
			 *
			 * @param name Name under which the code will be registered
			 * @param code Lua code
			 */
			void CompileAndAddString(const std::string &name, const std::string &code);

			/**
			 * @brief Compiles a string and adds it to the registry
			 *
			 * @details 
			 * Createas an instance of the LuaCompiler and uses it to compile
			 * the code. If the code is compiled sucessfully, it will be
			 * added to the registry under the provided name.
			 *
			 * If the `recompile` is set to true, the code will be compiled
			 * even if it already exists in the repository.
			 * The new code will be added in the place of the old.
			 *
			 * @param name Name under which the code will be registered
			 * @param code Lua code
			 * @param recompile if set to `true` the code will be recompiled, if already exists.
			 */
			void CompileAndAddString(const std::string &name, const std::string &code, bool recompile);
			
			/**
			 * @brief Compiles a file and adds it to the registry
			 *
			 * @details 
			 * Createas an instance of the LuaCompiler and uses it to compile
			 * the file. If the code is compiled sucessfully, it will be
			 * added to the registry under the provided name.
			 * 
			 * If the code snippet under this name already exists in the repository, 
			 * the method will ignore the new code provided in the call.
			 *
			 * @see CompileAndAddFile(std::string name, std::string code, bool recompile)
			 *
			 * @param name Name under which the code will be registered
			 * @param fname Name of the file
			 */
			void CompileAndAddFile(const std::string &name, const std::string &fname);
			
			/**
			 * @brief Compiles a file and adds it to the registry
			 *
			 * @details 
			 * Createas an instance of the LuaCompiler and uses it to compile
			 * the file. If the code is compiled sucessfully, it will be
			 * added to the registry under the provided name.
			 *
			 * If the `recompile` is set to true, the code will be compiled
			 * even if it already exists in the repository.
			 * The new code will be added in the place of the old.
			 *
			 * @param name Name under which the code will be registered
			 * @param code Lua code
			 * @param recompile if set to `true` the code will be recompiled, if already exists.
			 */
			void CompileAndAddFile(const std::string &name, const std::string &fname, bool recompile);

			/**
			 * @brief Checks if the snippet exists in the registry
			 *
			 * @details
			 * Check if the name exists in the registry
			 *
			 * @param name Name to be checked
			 *
			 * @return `true` if the name exists in the registry
			 */
			bool inline Exists(const std::string &name) {
				return !(registry.find( name ) == registry.end());
			}
			/**
			 * @brief Returns the code snipet associated with the name
			 *
			 * @details
			 * Returns the snippet associated with the name
			 *
			 * @param name Name of the snippet
			 *
			 * @return unique_ptr to the LuaCodeSnippet associatd with the name
			 */
			std::unique_ptr<LuaCodeSnippet> getByName(const std::string &name);
		};
	}
}
#endif // LUACPP_LUARGISTRY_HPP

