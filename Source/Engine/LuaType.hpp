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


#ifndef LUACPP_LUATYPE_HPP
#define LUACPP_LUATYPE_HPP

#include "../Lua.hpp"
#include "LuaState.hpp"

namespace LuaCpp {
	namespace Engine {
		class LuaType {
		   private:
			bool global;
			std::string globalName;
		   public:
			LuaType(); 
			virtual ~LuaType() = 0;
			virtual int getTypeId() = 0;
			virtual std::string getTypeName(LuaState &L) = 0;
			virtual void PushValue(LuaState &L) = 0;
			virtual void PopValue(LuaState &L, int idx) = 0;
			virtual std::string ToString() = 0;
			
			void PopValue(LuaState &L);
			void PushGlobal(LuaState &L, std::string global_name);
			void PopGlobal(LuaState &L);
			void PopGlobal(LuaState &L, std::string global_name);

			bool isGlobal();
			std::string getGlobalName();
		};
	}
}

#endif // LUACPP_LUATYPE_HPP
