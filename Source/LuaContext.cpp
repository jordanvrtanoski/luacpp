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

#include <stdexcept>
#include <iostream>
#include <filesystem>

#include "LuaContext.hpp"
#include "LuaVersion.hpp"

using namespace LuaCpp;
using namespace LuaCpp::Engine;
using namespace LuaCpp::Registry;


std::unique_ptr<LuaState> LuaContext::newState() {
	return newState(globalEnvironment);
}

std::unique_ptr<LuaState> LuaContext::newState(const LuaEnvironment &env) {
	std::unique_ptr<LuaState> L = std::make_unique<LuaState>();
	luaL_openlibs(*L);
	for(const auto &lib : libraries ) {
		((std::shared_ptr<LuaLibrary>) lib.second)->RegisterFunctions(*L);
	}
	for(const auto &var : env) {
		((std::shared_ptr<LuaType>) var.second)->PushGlobal(*L, var.first);
	}
	lua_pushstring(*L, std::string(LuaCpp::Version).c_str());
	lua_setglobal(*L, "_luacppversion");

	return L;
}

std::unique_ptr<LuaState> LuaContext::newStateFor(const std::string &name) {
	return newStateFor(name, globalEnvironment);
}

std::unique_ptr<LuaState> LuaContext::newStateFor(const std::string &name, const LuaEnvironment &env) {
	if (registry.Exists(name)) {
		std::unique_ptr<LuaCodeSnippet> cs = registry.getByName(name);
		std::unique_ptr<LuaState> L = newState(env);
		cs->UploadCode(*L);
		return L;	
	}	
	throw std::runtime_error("Error: The code snipped not found ...");
}

void LuaContext::CompileString(const std::string &name, const std::string &code) {
	registry.CompileAndAddString(name, code);
}

void LuaContext::CompileString(const std::string &name, const std::string &code, bool recompile) {
	registry.CompileAndAddString(name, code, recompile);
}

void LuaContext::CompileFile(const std::string &name, const std::string &fname) {
	registry.CompileAndAddFile(name,fname);
}

void LuaContext::CompileFile(const std::string &name, const std::string &fname, bool recompile) {
	registry.CompileAndAddFile(name,fname, recompile);
}

void LuaContext::CompileFolder(const std::string &path) {
	CompileFolder(path, "", false);
}

void LuaContext::CompileFolder(const std::string &path, const std::string &prefix) {
	CompileFolder(path, prefix, false);
}

void LuaContext::CompileFolder(const std::string &path, const std::string &prefix, bool recompile) {
	for (const auto &entry : std::filesystem::directory_iterator(path)) {
		if (entry.is_regular_file()){
			std::filesystem::path path = entry.path();
			if (path.extension() == ".lua") {
				try {
					if (prefix == "") {
						CompileFile(path.stem().native() ,path, recompile);
					} else {
						CompileFile(prefix+"."+path.stem().native() ,path, recompile);
					}
				} catch (std::logic_error &e) {
				}
			}
		}
	}
}

void LuaContext::CompileStringAndRun(const std::string &code) {
	registry.CompileAndAddString("default", code, true);
	Run("default");
}

void LuaContext::CompileFileAndRun(const std::string &code) {
	registry.CompileAndAddFile("default", code, true);
	Run("default");
}

void LuaContext::Run(const std::string &name) {
	RunWithEnvironment(name, globalEnvironment);
}

void LuaContext::RunWithEnvironment(const std::string &name, const LuaEnvironment &env) {
	std::unique_ptr<LuaState> L = newStateFor(name);
	
	for(const auto &var : env) {
		((std::shared_ptr<LuaType>) var.second)->PushGlobal(*L, var.first);
	}

	int res = lua_pcall(*L, 0, LUA_MULTRET, 0);
	if (res != LUA_OK ) {
		L->PrintStack(std::cout);
		throw std::runtime_error(lua_tostring(*L,1));
	}

	for(const auto &var : env) {
		((std::shared_ptr<LuaType>) var.second)->PopGlobal(*L);
	}

}
		
void LuaContext::AddLibrary(std::shared_ptr<Registry::LuaLibrary> &library) {
	libraries[library->getName()] = std::move(library);
}

void LuaContext::AddGlobalVariable(const std::string &name, std::shared_ptr<Engine::LuaType> var) {
	globalEnvironment[name] = std::move(var);
}

std::shared_ptr<Engine::LuaType> &LuaContext::getGlobalVariable(const std::string &name) {
	return globalEnvironment[name];
}

