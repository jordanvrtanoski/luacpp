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


#ifndef LUACPP_LUATBOOLEAN_HPP
#define LUACPP_LUATBOOLEAN_HPP

#include "../Lua.hpp"
#include "LuaState.hpp"
#include "LuaType.hpp"

namespace LuaCpp {
	namespace Engine {
		/**
		 * @brief Implementation of the LUA_TBOOLEAN type
		 *
		 * @details
		 * Implementation of the LUA_TBOOLEAN type.
		 */
		class LuaTBoolean : public LuaType {
	           private:
			/**
			 * @brief the boolean value
			 */
			bool value;
		   public:
			/**
			 * @brief Explicit constructor with a boolean value
			 *
			 * @details
			 * Explicit constructor with the given boolean value.
			 */
			explicit LuaTBoolean(bool _value) : LuaType(), value(std::move(_value)) {}

			/**
			 * @brief Default destructor
			 */
			~LuaTBoolean() {}

			/**
			 * @brief Returns the type id as deifend in the `lua.h`
			 *
			 * @detail
			 * Returns the lua type id for the nil type: `LUA_TBOOLEAN`
			 *
			 * @see LuaType.getTypeId()
			 */
			int getTypeId() const;

			/**
			 * @brief Returns the string representation of the type
			 *
			 * @details
			 * Returns the string interpretation of the LUA_TBOOLEAN type.
			 *
			 * @see LuaType.getTypeName()
			 *
			 */
			std::string getTypeName(LuaState &L) const;

			/**
			 * @brief Pushes the value on the top fo the stack
			 *
			 * @details
			 * Pushes the value on top of the stack
			 *
			 * @see LuaType.PushValue()
			 */
			void PushValue(LuaState &L);

			/**
			 * @brief Reads the value from the stack
			 *
			 * @details
			 * Reads a vlue from the stack. The method will check if the stack
			 * on the index has the LUA_TBOOLEAN type, and if not will throw an error.
			 * If the value is LUA_TBOOLEAN, the value will be read from the `lua` context
			 *
			 * @see LuaType.PopValue()
			 */
			using LuaType::PopValue;
			void PopValue(LuaState &L, int idx);

			/**
			 * @brief Returns string with the value `true`/`false`
			 *
			 * @see LuaType.ToString()
			 */
			std::string ToString() const ;

			/**
			 * @brief Returns the value of the boolan
			 *
			 * @details
			 * Returns the value of the boolean variable
			 *
			 * @return
			 * boolean value encapsulated by the class
			 */
			bool getValue() const;

			/**
			 * @brief Set the boolean value
			 *
			 * @details
			 * Sets the value of the boolean variable
			 *
			 * @param value value that will be encapsulated
			 */
			void setValue(bool value);
		};
	}
}
#endif // LUACPP_LUATBOOLEAN_HPP

