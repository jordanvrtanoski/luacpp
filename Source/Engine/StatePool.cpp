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

#include "StatePool.hpp"
#include "LuaTNil.hpp"
#include "LuaTString.hpp"
#include "LuaTNumber.hpp"
#include "LuaTBoolean.hpp"

using namespace LuaCpp::Engine;

StatePool::StatePool(std::string color, PoolConfig config)
	: color_(std::move(color))
	, config_(std::move(config))
	, available_()
	, currentSize_(0)
	, checkedOut_(0)
	, threadSafe_(false)
{
}

std::unique_ptr<LuaState> StatePool::createState() {
	auto state = std::make_unique<LuaState>();
	
	loadLibraries(*state);
	loadGlobals(*state);
	loadHooks(*state);
	
	return std::move(state);
}

void StatePool::loadLibraries(LuaState& state) {
	if (config_.libraries.empty()) {
		luaL_openlibs(state);
		return;
	}

	luaL_requiref(state, "_G", luaopen_base, 1);
	lua_pop(state, 1);

	for (const auto& libName : config_.libraries) {
		if (libName == "base") {
			continue;
		} else if (libName == "coroutine") {
			luaL_requiref(state, "coroutine", luaopen_coroutine, 1);
			lua_pop(state, 1);
		} else if (libName == "debug") {
			luaL_requiref(state, "debug", luaopen_debug, 1);
			lua_pop(state, 1);
		} else if (libName == "io") {
			luaL_requiref(state, "io", luaopen_io, 1);
			lua_pop(state, 1);
		} else if (libName == "math") {
			luaL_requiref(state, "math", luaopen_math, 1);
			lua_pop(state, 1);
		} else if (libName == "os") {
			luaL_requiref(state, "os", luaopen_os, 1);
			lua_pop(state, 1);
		} else if (libName == "package") {
			luaL_requiref(state, "package", luaopen_package, 1);
			lua_pop(state, 1);
		} else if (libName == "string") {
			luaL_requiref(state, "string", luaopen_string, 1);
			lua_pop(state, 1);
		} else if (libName == "table") {
			luaL_requiref(state, "table", luaopen_table, 1);
			lua_pop(state, 1);
		} else if (libName == "utf8") {
			luaL_requiref(state, "utf8", luaopen_utf8, 1);
			lua_pop(state, 1);
		}
	}
}

void StatePool::loadGlobals(LuaState& state) {
	for (const auto& var : config_.globalVariables) {
		var.second->PushGlobal(state, var.first);
	}
}

void StatePool::loadHooks(LuaState& state) {
	for (const auto& hook : config_.hooks) {
		int mask = 0;
		const std::string& hookType = std::get<0>(hook);
		int count = std::get<1>(hook);
		lua_Hook hookFunc = std::get<2>(hook);

		if (hookType == "call") {
			mask = LUA_MASKCALL;
		} else if (hookType == "return") {
			mask = LUA_MASKRET;
		} else if (hookType == "line") {
			mask = LUA_MASKLINE;
		} else if (hookType == "count") {
			mask = LUA_MASKCOUNT;
		}

		lua_sethook(state, hookFunc, mask, count);
	}
}

void StatePool::resetState(LuaState& state) {
	lua_settop(state, 0);
	
	lua_sethook(state, nullptr, 0, 0);
	
	for (const auto& var : config_.globalVariables) {
		var.second->PushGlobal(state, var.first);
	}
}

std::unique_ptr<LuaState> StatePool::acquire() {
	std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
	if (threadSafe_) {
		lock.lock();
	}

	if (!available_.empty()) {
		std::unique_ptr<LuaState> state = std::move(available_.front());
		available_.pop();
		checkedOut_++;
		return std::move(state);
	}

	if (currentSize_ < config_.maxSize) {
		currentSize_++;
		checkedOut_++;
		if (threadSafe_) {
			lock.unlock();
		}
		return createState();
	}

	throw PoolExhaustedException(color_);
}

void StatePool::release(std::unique_ptr<LuaState> state) {
	std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
	if (threadSafe_) {
		lock.lock();
	}

	checkedOut_--;

	resetState(*state);
	available_.push(std::move(state));

	if (threadSafe_) {
		cv_.notify_one();
	}
}

void StatePool::warmup(size_t n) {
	size_t toCreate = std::min(n, config_.maxSize - currentSize_);
	
	for (size_t i = 0; i < toCreate; i++) {
		auto state = createState();
		
		std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
		if (threadSafe_) {
			lock.lock();
		}
		
		currentSize_++;
		available_.push(std::move(state));
	}
}

void StatePool::drain() {
	std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
	if (threadSafe_) {
		lock.lock();
	}

	while (!available_.empty()) {
		available_.pop();
		currentSize_--;
	}
}

const std::string& StatePool::getColor() const {
	return color_;
}

const PoolConfig& StatePool::getConfig() const {
	return config_;
}

size_t StatePool::getMaxSize() const {
	return config_.maxSize;
}

size_t StatePool::getCurrentSize() const {
	std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
	if (threadSafe_) {
		lock.lock();
	}
	return currentSize_;
}

size_t StatePool::availableCount() const {
	std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
	if (threadSafe_) {
		lock.lock();
	}
	return available_.size();
}

size_t StatePool::checkedOutCount() const {
	std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
	if (threadSafe_) {
		lock.lock();
	}
	return checkedOut_;
}

void StatePool::setThreadSafe(bool threadSafe) {
	threadSafe_ = threadSafe;
}

bool StatePool::isThreadSafe() const {
	return threadSafe_;
}
