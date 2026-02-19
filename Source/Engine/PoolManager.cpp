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

#include "PoolManager.hpp"

using namespace LuaCpp::Engine;

PoolManager::PoolManager()
	: pools_()
	, threadSafe_(false)
{
	initializePredefinedPools();
}

void PoolManager::initializePredefinedPools() {
	PoolConfig defaultConfig;
	defaultConfig.libraries = {};
	defaultConfig.maxSize = 5;
	pools_["default"] = std::make_unique<StatePool>("default", defaultConfig);

	PoolConfig sandboxedConfig;
	sandboxedConfig.libraries = {"base", "math", "string", "table"};
	sandboxedConfig.maxSize = 5;
	pools_["sandboxed"] = std::make_unique<StatePool>("sandboxed", sandboxedConfig);

	PoolConfig minimalConfig;
	minimalConfig.libraries = {"base"};
	minimalConfig.maxSize = 5;
	pools_["minimal"] = std::make_unique<StatePool>("minimal", minimalConfig);

	PoolConfig ioConfig;
	ioConfig.libraries = {"base", "io", "os"};
	ioConfig.maxSize = 5;
	pools_["io"] = std::make_unique<StatePool>("io", ioConfig);
}

std::map<std::string, std::unique_ptr<StatePool>>::iterator PoolManager::findPoolOrThrow(const std::string& color) {
	auto it = pools_.find(color);
	if (it == pools_.end()) {
		throw std::runtime_error("Pool '" + color + "' not found");
	}
	return it;
}

StatePool& PoolManager::getPool(const std::string& color) {
	std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
	if (threadSafe_) {
		lock.lock();
	}

	return *(findPoolOrThrow(color)->second);
}

StatePool& PoolManager::createPool(const std::string& color, const PoolConfig& config) {
	std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
	if (threadSafe_) {
		lock.lock();
	}

	auto it = pools_.find(color);
	if (it != pools_.end()) {
		throw std::runtime_error("Pool '" + color + "' already exists");
	}

	auto pool = std::make_unique<StatePool>(color, config);
	pool->setThreadSafe(threadSafe_);
	
	StatePool& ref = *pool;
	pools_[color] = std::move(pool);
	
	return ref;
}

void PoolManager::destroyPool(const std::string& color) {
	std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
	if (threadSafe_) {
		lock.lock();
	}

	auto it = findPoolOrThrow(color);

	if (color == "default" || color == "sandboxed" || color == "minimal" || color == "io") {
		throw std::runtime_error("Cannot destroy predefined pool '" + color + "'");
	}

	pools_.erase(it);
}

bool PoolManager::hasPool(const std::string& color) const {
	std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
	if (threadSafe_) {
		lock.lock();
	}

	return pools_.find(color) != pools_.end();
}

std::vector<std::string> PoolManager::listPools() const {
	std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
	if (threadSafe_) {
		lock.lock();
	}

	std::vector<std::string> result;
	for (const auto& pair : pools_) {
		result.push_back(pair.first);
	}
	return result;
}

void PoolManager::setThreadSafe(bool threadSafe) {
	std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
	if (threadSafe_) {
		lock.lock();
	}

	threadSafe_ = threadSafe;
	
	for (auto& pair : pools_) {
		pair.second->setThreadSafe(threadSafe);
	}
}

bool PoolManager::isThreadSafe() const {
	return threadSafe_;
}
