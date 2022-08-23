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


#ifndef LUACPP_LUATSTRING_HPP
#define LUACPP_LUATSTRING_HPP

#include <ostream>

#include "../Lua.hpp"
#include "LuaState.hpp"
#include "LuaType.hpp"

namespace LuaCpp {
	namespace Engine {
		/**
		 * @brief Implementation of the LUA_TSTRING type
		 *
		 * @details
		 * Implementation of the LUA_TSTRING type. The `lua` engin is treating the
		 * string as `char *`, howeve the class will convert it to `std::string` and
		 * store it as such.
		 */
		class LuaTString : public LuaType{
		   private:
			/**
			 * The encapsulated string value
			 */
			std::string value;
		   public:

			/**
			 * @brief Explicit constructor accepting the `std::string` value to be encapsulated
			 *
			 * @details
			 * Expicit constructor accepting a `std::string` value to be encapsulated 
			 * and assigning it to the internal storage member.
			 *
			 * @param value the double value that will be assigned to the type
			 */
			explicit LuaTString(std::string _value) : LuaType(), value(std::move(_value)) {}
			
			/**
			 * @brief Default destructor
			 */
			~LuaTString() {}

			/**
			 * @brief Returns the type id as deifend in the `lua.h`
			 *
			 * @detail
			 * Returns the lua type id for the nil type: `LUA_TSTRING`
			 *
			 * @see LuaType.getTypeId()
			 */
			int getTypeId() const;

			/**
			 * @brief Returns the string representation of the type
			 *
			 * @details
			 * Returns the string interpretation of the LUA_TSTRING type.
			 *
			 * @see LuaType.getTypeName()
			 *
			 */
			std::string getTypeName(LuaState &L) const;

			/**
			 * @brief Pushes the number on the top fo the stack
			 *
			 * @details
			 * Pushes a number on top of the stack
			 *
			 * @see LuaType.PushValue()
			 */
			void PushValue(LuaState &L);

			/**
			 * @brief Reads the value from the stack
			 *
			 * @details
			 * Reads a vlue from the stack. The method will check if the stack
			 * on the index has the LUA_TSTRING type, and if not will throw an error.
			 * If the value is LUA_TSTRING, the value will be read from the `lua` context 
			 *
			 * @see LuaType.PopValue()
			 */
			using LuaType::PopValue;
			void PopValue(LuaState &L, int idx);

			/**
			 * @brief Returns the `to_string()` from the double value
			 *
			 * @details
			 * Returns the string with the double value
			 */
			std::string ToString() const;

			/**
			 * @brief Returns the value of the std::string 
			 *
			 * @details
			 * Returns the value of the std::string variable
			 *
			 * @return
			 * double value encapsulated by the class
			 */
			std::string getValue() const;

			/**
			 * @brief Set the double value
			 *
			 * @details
			 * Sets the value of the std::string variable
			 *
			 * @param value value that will be encapsulated
			 */
			void setValue(std::string value);
		};
	}
}

#endif // LUACPP_LUATSTRING_HPP
