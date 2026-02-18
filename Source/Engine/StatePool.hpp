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

#ifndef LUACPP_STATEPOOL_HPP
#define LUACPP_STATEPOOL_HPP

#include <string>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <stdexcept>

#include "../Lua.hpp"
#include "LuaState.hpp"
#include "PoolConfig.hpp"

namespace LuaCpp {
	namespace Engine {

		class PoolExhaustedException : public std::runtime_error {
		public:
			explicit PoolExhaustedException(const std::string& color)
				: std::runtime_error("Pool '" + color + "' exhausted: no available states and maximum size reached") {}
		};

		class StatePool {
		private:
			std::string color_;
			PoolConfig config_;
			std::queue<std::unique_ptr<LuaState>> available_;
			size_t currentSize_ = 0;
			size_t checkedOut_ = 0;
			bool threadSafe_ = false;
			mutable std::mutex mutex_;
			std::condition_variable cv_;

			std::unique_ptr<LuaState> createState();
			void resetState(LuaState& state);
			void loadLibraries(LuaState& state);
			void loadGlobals(LuaState& state);
			void loadHooks(LuaState& state);

		public:
			explicit StatePool(std::string color, PoolConfig config);
			~StatePool() = default;

			StatePool(const StatePool&) = delete;
			StatePool& operator=(const StatePool&) = delete;
			StatePool(StatePool&&) = delete;
			StatePool& operator=(StatePool&&) = delete;

			std::unique_ptr<LuaState> acquire();
			void release(std::unique_ptr<LuaState> state);

			void warmup(size_t n);
			void drain();

			const std::string& getColor() const;
			const PoolConfig& getConfig() const;
			size_t getMaxSize() const;
			size_t getCurrentSize() const;
			size_t availableCount() const;
			size_t checkedOutCount() const;

			void setThreadSafe(bool threadSafe);
			bool isThreadSafe() const;
		};
	}
}

#endif // LUACPP_STATEPOOL_HPP
