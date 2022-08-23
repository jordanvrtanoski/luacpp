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


#ifndef LUACPP_LUATNIL_HPP
#define LUACPP_LUATNIL_HPP

#include "../Lua.hpp"
#include "LuaState.hpp"
#include "LuaType.hpp"

namespace LuaCpp {
	namespace Engine {
		/**
		 * @brief Implementation of the LUA_TNIL type
		 *
		 * @details
		 * Implmenetation of the `nil` type of lua engine.
		 * The class is inheriting from `LuaType`
		 */
		class LuaTNil : public LuaType {
		   public:
			/**
			 * @bried Default constructor
			 *
			 * @details
			 * Default constructor for the class
			 */
			LuaTNil() : LuaType() {}

			/**
			 * @brief Default destructor
			 *
			 * @details
			 * Default destructor for the class
			 */
			~LuaTNil() {}
			
			/**
			 * @brief Returns the type id as deifend in the `lua.h`
			 *
			 * @detail
			 * Returns the lua type id for the nil type: `LUA_TNIL`
			 *
			 * @see LuaType.getTypeId()
			 */
			int getTypeId() const;

			/**
			 * @brief Returns the string representation of the type
			 *
			 * @details
			 * Returns the string interpretation of the nil type.
			 *
			 * @see LuaType.getTypeName()
			 *
			 */
			std::string getTypeName(LuaState &L) const;

			/**
			 * @brief Pushes a LUA_TNIL on the top fo the stack
			 *
			 * @details
			 * Pushes a LUA_TNIL on top of the stack
			 *
			 * @see LuaType.PushValue()
			 */
			void PushValue(LuaState &L);

			/**
			 * @brief Reads the value from the stack
			 *
			 * @details
			 * Reads a vlue from the stack. The method will check if the stack
			 * on the index has the LUA_TNIL type, and if not will throw an error.
			 *
			 * @see LuaType.PopValue()
			 */
			using LuaType::PopValue;
			void PopValue(LuaState &L, int idx);

			/**
			 * @brief Returns string with `nil`
			 *
			 * @see LuaType.ToString()
			 */
			std::string ToString() const;
		};
	}
}
#endif // LUACPP_LUATNIL_HPP
