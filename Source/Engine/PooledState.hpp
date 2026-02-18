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

#ifndef LUACPP_POOLEDSTATE_HPP
#define LUACPP_POOLEDSTATE_HPP

#include <memory>

#include "LuaState.hpp"
#include "StatePool.hpp"

namespace LuaCpp {
	namespace Engine {

		class PooledState {
		private:
			std::unique_ptr<LuaState> state_;
			StatePool* pool_;
			bool released_ = false;

		public:
			PooledState(std::unique_ptr<LuaState> state, StatePool* pool)
				: state_(std::move(state))
				, pool_(pool)
				, released_(false)
			{}

			PooledState(PooledState&& other) noexcept
				: state_(std::move(other.state_))
				, pool_(other.pool_)
				, released_(other.released_)
			{
				other.released_ = true;
			}

			PooledState& operator=(PooledState&& other) noexcept {
				if (this != &other) {
					if (!released_ && state_ && pool_) {
						pool_->release(std::move(state_));
					}
					state_ = std::move(other.state_);
					pool_ = other.pool_;
					released_ = other.released_;
					other.released_ = true;
				}
				return *this;
			}

			PooledState(const PooledState&) = delete;
			PooledState& operator=(const PooledState&) = delete;

			~PooledState() {
				if (!released_ && state_ && pool_) {
					pool_->release(std::move(state_));
				}
			}

			LuaState& operator*() {
				return *state_;
			}

			const LuaState& operator*() const {
				return *state_;
			}

			LuaState* operator->() {
				return state_.get();
			}

			const LuaState* operator->() const {
				return state_.get();
			}

			LuaState* get() {
				return state_.get();
			}

			const LuaState* get() const {
				return state_.get();
			}

			explicit operator bool() const {
				return state_ != nullptr;
			}

			void release() {
				if (!released_ && state_ && pool_) {
					pool_->release(std::move(state_));
					released_ = true;
				}
			}
		};
	}
}

#endif // LUACPP_POOLEDSTATE_HPP
