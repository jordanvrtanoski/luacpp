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

#ifndef LUACPP_POOLMANAGER_HPP
#define LUACPP_POOLMANAGER_HPP

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <mutex>

#include "StatePool.hpp"
#include "PoolConfig.hpp"

namespace LuaCpp {
	namespace Engine {

		class PoolManager {
		private:
			std::map<std::string, std::unique_ptr<StatePool>> pools_;
			bool threadSafe_ = false;
			mutable std::mutex mutex_;

			void initializePredefinedPools();

		public:
			PoolManager();
			~PoolManager() = default;

			PoolManager(const PoolManager&) = delete;
			PoolManager& operator=(const PoolManager&) = delete;
			PoolManager(PoolManager&&) = delete;
			PoolManager& operator=(PoolManager&&) = delete;

			StatePool& getPool(const std::string& color);
			StatePool& createPool(const std::string& color, const PoolConfig& config);
			void destroyPool(const std::string& color);
			bool hasPool(const std::string& color) const;
			std::vector<std::string> listPools() const;

			void setThreadSafe(bool threadSafe);
			bool isThreadSafe() const;
		};
	}
}

#endif // LUACPP_POOLMANAGER_HPP
