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

#ifndef LUACPP_POOLCONFIG_HPP
#define LUACPP_POOLCONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <tuple>

#include "../Lua.hpp"
#include "LuaType.hpp"

namespace LuaCpp {
	namespace Engine {

		typedef std::map<std::string, std::shared_ptr<LuaType>> PoolEnvironment;

		struct PoolConfig final {
			std::vector<std::string> libraries;
			PoolEnvironment globalVariables;
			std::vector<std::tuple<std::string, int, lua_Hook>> hooks;
			size_t maxSize = 5;
			size_t exhaustionTimeoutMs = 0;

			PoolConfig() = default;

			PoolConfig& SetLibraries(const std::vector<std::string>& libs) {
				libraries = libs;
				return *this;
			}

			PoolConfig& AddGlobalVariable(const std::string& name, std::shared_ptr<LuaType> value) {
				globalVariables[name] = std::move(value);
				return *this;
			}

			PoolConfig& AddHook(const std::string& hookType, int count, lua_Hook hookFunc) {
				hooks.emplace_back(hookType, count, hookFunc);
				return *this;
			}

			PoolConfig& SetMaxSize(size_t size) {
				maxSize = size;
				return *this;
			}

			PoolConfig& SetExhaustionTimeoutMs(size_t timeoutMs) {
				exhaustionTimeoutMs = timeoutMs;
				return *this;
			}
		};
	}
}

#endif // LUACPP_POOLCONFIG_HPP
