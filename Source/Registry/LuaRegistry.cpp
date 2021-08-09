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

#include "LuaRegistry.hpp"
#include "LuaCompiler.hpp"


using namespace LuaCpp::Registry;

void LuaRegistry::CompileAndAddString(const std::string &name, const std::string &code) {
	CompileAndAddString(name, code, false);
}

void LuaRegistry::CompileAndAddString(const std::string &name, const std::string &code, bool recompile) {

	if ( !Exists(name) or recompile ) { 
		LuaCompiler cmp;
		std::unique_ptr<LuaCodeSnippet> snp = cmp.CompileString(name, code);

		registry[name] = std::move(*snp);
	}
}

void LuaRegistry::CompileAndAddFile(const std::string &name, const std::string &fname) {
	CompileAndAddFile(name, fname, false);
}

void LuaRegistry::CompileAndAddFile(const std::string &name, const std::string &fname, bool recompile) {

	if ( !Exists(name) or recompile ) { 
		LuaCompiler cmp;
		std::unique_ptr<LuaCodeSnippet> snp = cmp.CompileFile(name, fname);

		registry[name] = std::move(*snp);
	}
}

std::unique_ptr<LuaCodeSnippet> LuaRegistry::getByName(const std::string &name) {
	return std::make_unique<LuaCodeSnippet>(registry[name]);
}
