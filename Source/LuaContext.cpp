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

#include "LuaContext.hpp"
#include <stdexcept>
#include <iostream>

using namespace LuaCpp;
using namespace LuaCpp::Engine;
using namespace LuaCpp::Registry;


std::unique_ptr<LuaState> LuaContext::newState() {
	std::unique_ptr<LuaState> L = std::make_unique<LuaState>();
	luaL_openlibs(*L);
	for(const auto &lib : libraries ) {
		((LuaLibrary) lib.second).RegisterFunctions(*L);
	}
	return L;
}

std::unique_ptr<LuaState> LuaContext::newStateFor(std::string name) {
	if (registry.Exists(name)) {
		std::unique_ptr<LuaCodeSnippet> cs = registry.getByName(name);
		std::unique_ptr<LuaState> L = newState();
		cs->UploadCode(*L);
		return L;	
	}	
	throw std::runtime_error("Error: The code snipped not found ...");
}

void LuaContext::CompileString(std::string name, std::string code) {
	registry.CompileAndAddString(name, code);
}

void LuaContext::CompileString(std::string name, std::string code, bool recompile) {
	registry.CompileAndAddString(name, code, recompile);
}

void LuaContext::CompileFile(std::string name, std::string fname) {
	registry.CompileAndAddFile(name,fname);
}

void LuaContext::CompileFile(std::string name, std::string fname, bool recompile) {
	registry.CompileAndAddFile(name,fname, recompile);
}

void LuaContext::CompileStringAndRun(std::string code) {
	registry.CompileAndAddString("default", code, true);
	Run("default");
}

void LuaContext::CompileFileAndRun(std::string code) {
	registry.CompileAndAddFile("default", code, true);
	Run("default");
}

void LuaContext::Run(std::string name) {
	std::unique_ptr<LuaState> L = newStateFor(name);
	
	int res = lua_pcall(*L, 0, LUA_MULTRET, 0);
	if (res != LUA_OK ) {
		L->PrintStack(std::cout);
		throw std::runtime_error(lua_tostring(*L,1));
	}

}
		

void LuaContext::AddLibrary(std::unique_ptr<Registry::LuaLibrary> library) {
	libraries.insert(std::make_pair<std::string, LuaLibrary>(library->getName(), std::move(*library)));
}
