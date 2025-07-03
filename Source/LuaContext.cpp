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


std::unique_ptr<LuaState> LuaContext::newState(std::optional<Engine::StateParams> params) {
	return newState(globalEnvironment, params);
}

std::unique_ptr<LuaState> LuaContext::newState(const LuaEnvironment &env, std::optional<Engine::StateParams> params) {
	std::unique_ptr<LuaState> L = nullptr;
	if (params) {
		L = std::make_unique<LuaState>(*params);
	} else {
		L = std::make_unique<LuaState>();
	}
	luaL_openlibs(*L);

	for(const auto &lib : libraries ) {
		((std::shared_ptr<LuaLibrary>) lib.second)->RegisterFunctions(*L);
	}

	registerHooks(*L);

	for(const auto &var : env) {
		((std::shared_ptr<LuaType>) var.second)->PushGlobal(*L, var.first);
	}

	// push new lua built-in functions to global:
	for(std::pair<const std::string, LuaCpp::Registry::LuaCFunction> builtInFunction : builtInFunctions)
	{	
		// Check if the CFunction is valid before pushing
		if (builtInFunction.second.getCFunction() == nullptr) {
			throw std::runtime_error("Attempted to register a null C function: " + builtInFunction.first);
		}
		lua_pushcfunction(*L, builtInFunction.second.getCFunction());
		lua_setglobal(*L, builtInFunction.first.c_str());
	}

	lua_pushstring(*L, std::string(LuaCpp::Version).c_str());
	lua_setglobal(*L, "_luacppversion");

	return L;
}

std::unique_ptr<LuaState> LuaContext::newStateFor(const std::string &name, std::optional<Engine::StateParams> params) {
	return newStateFor(name, globalEnvironment, params);
}

std::unique_ptr<LuaState> LuaContext::newStateFor(const std::string &name, const LuaEnvironment &env, std::optional<Engine::StateParams> params) {
	if (registry.Exists(name)) {
		std::unique_ptr<LuaCodeSnippet> cs = registry.getByName(name);
		std::unique_ptr<LuaState> L = newState(env, params);
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

StateProxy LuaContext::CreateStateFor(const std::string &name, std::optional<Engine::StateParams> params) {
	std::unique_ptr<LuaState> L = newStateFor(name, params);
	return StateProxy(std::move(L));
}

void StateProxy::RunWithEnvironment(const LuaEnvironment &env) {
	for(const auto &var : env) {
		((std::shared_ptr<LuaType>) var.second)->PushGlobal(*state_, var.first);
	}
	int res = lua_pcall(*state_, 0, LUA_MULTRET, 0);
	if (res != LUA_OK ) {
		state_->PrintStack(std::cout);
		throw std::runtime_error(lua_tostring(*state_,1));
	}
	for(const auto &var : env) {
		((std::shared_ptr<LuaType>) var.second)->PopGlobal(*state_);
	}
}

void LuaContext::RunWithEnvironment(const std::string &name, const LuaEnvironment &env, std::optional<Engine::StateParams> params) {
	std::unique_ptr<LuaState> L = newStateFor(name, params);

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

std::shared_ptr<Registry::LuaLibrary> LuaContext::getStdLibrary(const std::string &libName)
{
	std::shared_ptr<LuaLibrary> foundLibrary = NULL;
	std::unique_ptr<LuaCpp::Engine::LuaState> L = newState(globalEnvironment);
	lua_getglobal(*L, libName.c_str());

	if(lua_istable(*L, 1))
	{
		// get metatable-name for corresponding standard-library:
		if(libName == "io")
		{
			foundLibrary = std::make_shared<LuaLibrary>(libName, LUA_FILEHANDLE);
		}

		else
		{
			foundLibrary = std::make_shared<LuaLibrary>(libName);
		}

		lua_pushnil(*L);

		while(lua_next(*L, -2) != 0)
		{
			foundLibrary->AddCFunction(lua_tostring(*L, -2), lua_tocfunction(*L, -1));

			lua_pop(*L, 1);
		}

		// check, if for this library a metatable exists at all:
		if(luaL_getmetatable(*L, foundLibrary->getMetaTableName().c_str()))
		{
			// get meta-methods of library:
			// ============================
			lua_pushnil(*L);

			while(lua_next(*L, -2) != 0)
			{
				// check, if it is a meta-method:
				if(lua_iscfunction(*L, -1))
				{
					foundLibrary->AddCMethod(lua_tostring(*L, -2), lua_tocfunction(*L, -1));
				}

				lua_pop(*L, 1);
			}

			// get methods of library:
			// =======================
			lua_getfield(*L, -1, "__index");
			lua_pushnil(*L);

			while(lua_next(*L, -2) != 0)
			{
				foundLibrary->AddCMethod(lua_tostring(*L, -2), lua_tocfunction(*L, -1));

				lua_pop(*L, 1);
			}
		}
	}

	return foundLibrary;
}

std::shared_ptr<Registry::LuaCFunction> LuaContext::getBuiltInFnc(const std::string &fncName)
{
	std::shared_ptr<Registry::LuaCFunction> builtInFnc = NULL;
	std::unique_ptr<LuaCpp::Engine::LuaState> L = newState(globalEnvironment);

	lua_getglobal(*L, fncName.c_str());

	// check, if it is a function at all:
	if(lua_iscfunction(*L, 1))
	{
		builtInFnc = std::shared_ptr<Registry::LuaCFunction>(new Registry::LuaCFunction(lua_tocfunction(*L, 1)));
	}

	return builtInFnc;
}

void LuaContext::setBuiltInFnc(const std::string &fncName, lua_CFunction cfunction)
{
	setBuiltInFnc(fncName, cfunction, false);
}

void LuaContext::setBuiltInFnc(const std::string &fncName, lua_CFunction cfunction, bool replace)
{
	std::unique_ptr<LuaCpp::Engine::LuaState> L = newState(globalEnvironment);

	// if we want to replace already existing function, we have to remove old one from list first:
	if(replace)
	{
		builtInFunctions.erase(fncName);

		// check, if function still exists in global environment, so we delete it there:
		if(Exists_buildInFnc(*L, fncName))
		{
			lua_pushnil(*L);
			lua_setglobal(*L, fncName.c_str());
		}
	}

	// check, if function already exists:
	if(!Exists_buildInFnc(*L, fncName))
	{
		std::unique_ptr<LuaCFunction> func = std::make_unique<LuaCFunction>(cfunction);
		func->setName(fncName);
		builtInFunctions.insert(std::make_pair(fncName, std::move(*func)));
	}
}

bool LuaContext::Exists_buildInFnc(Engine::LuaState &L, const std::string &fncName) 
{
	lua_getglobal(L, fncName.c_str());

	return !(builtInFunctions.find( fncName ) == builtInFunctions.end()) || (!lua_isnil(L, -1));
}
		
void LuaContext::AddLibrary(const std::shared_ptr<Registry::LuaLibrary> &library) {
		if (!library) {
			throw std::invalid_argument("Library cannot be null");
		}
		this->libraries[library->getName()] = library;	
}

void LuaContext::AddGlobalVariable(const std::string &name, const std::shared_ptr<Engine::LuaType> var) {
	if (!var) {
		throw std::invalid_argument("Variable cannot be null");
	}
	globalEnvironment[name] = std::move(var);
}

std::shared_ptr<Engine::LuaType> &LuaContext::getGlobalVariable(const std::string &name) {
    auto it = globalEnvironment.find(name);
    if (it == globalEnvironment.end()) {
        static std::shared_ptr<Engine::LuaType> nullPtr = nullptr;
        return nullPtr;
    }
    return it->second;
}

void LuaContext::addHook(lua_Hook hookFunc, const std::string &hookType, const int count)
{
	hooks.push_back(std::tuple<std::string, int, lua_Hook>(hookType, count, hookFunc));
}

void LuaContext::registerHooks(LuaCpp::Engine::LuaState &L)
{
	for(const auto &hook : hooks) 
	{
		int mask;
		int count = std::get<1>(hook);

		if(std::get<0>(hook) == "call")
		{
			mask = LUA_MASKCALL;
		}

		else if(std::get<0>(hook) == "return")
		{
			mask = LUA_MASKRET;
		}

		else if(std::get<0>(hook) == "line")
		{
			mask = LUA_MASKLINE;
		}

		else if(std::get<0>(hook) == "count")
		{
			mask = LUA_MASKCOUNT;
		}

		lua_sethook(L, std::get<2>(hook), mask, count);
	}
}