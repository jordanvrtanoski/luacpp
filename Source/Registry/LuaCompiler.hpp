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

#ifndef LUACPP_COMPILER_HPP
#define LUACPP_COMPILER_HPP

#include <memory>

#include "LuaCodeSnippet.hpp"

namespace LuaCpp {
	namespace Registry {
		
		/**
		 * @brief Lua compiler
		 *
		 * @details
		 * Compiler that uses the luac functions to generate Lua binary code
		 * in a format of the LuaCodeSnippet.
		 *
		 * By compiling the code and storing it as a binary buffer, the
		 * LuaCpp is improving the performance of the re-execution of the same code.
		 */
		class LuaCompiler {
		    public:
			/**
			 * @brief Default constructor
			 */
			explicit LuaCompiler() {}

			/** 
			 * @brief Default destructor
			 */
                        ~LuaCompiler() {}

			/**
			 * @brief Compiles a lua code given by a string
			 *
			 * @details
			 * Compiles a lua code given by the string passed to the compiler.
			 *
			 * @param name Name of the generated LuaCodeSnippet
			 * @param code Lua code
			 *
			 * @return LuaCodeSnippet containing the binray form of the code
			 */
			std::unique_ptr<LuaCodeSnippet> CompileString(std::string name, std::string code);

			/**
			 * @brief Compiles a lua file
			 *
			 * @details
			 * Loads the file from the disk and compiles it in a lua binary code
			 *
			 * @param name Name of the generated LuaCodeSnippet
			 * @param fname Name of the file 
			 *
			 * @return LuaCodeSippet containing te binary form of the code
			 */
			std::unique_ptr<LuaCodeSnippet> CompileFile(std::string name, std::string fname);
		};
	}
}

#endif // LUACPP_COMPILER_HPP
