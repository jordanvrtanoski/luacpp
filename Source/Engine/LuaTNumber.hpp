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


#ifndef LUACPP_LUATNUMBER_HPP
#define LUACPP_LUATNUMBER_HPP

#include "../Lua.hpp"
#include "LuaState.hpp"
#include "LuaType.hpp"

namespace LuaCpp {
	namespace Engine {
		/**
		 * @brief Implementation of the LUA_TNUMBER type
		 *
		 * @details
		 * Implementation of the LUA_TNUMBER type. The `lua` engin is treating the
		 * `number` type as a `float` or as a `double`, depending on the way in which
		 * the library was compiled. Internally, the `double` is used and the user should
		 * confirm if the library is using a `float` which might lead toward loss of data accuracy
		 * by checking the type of the `lua_Number` 
		 */
		class LuaTNumber : public LuaType {
	           private:
			double value;
		   public:
			/**
			 * @brief Explicit constructor accepting the `double` value
			 * and assigning it to the internal storage member.
			 *
			 * @details
			 * Expicit constructor accepting a `double` value and assigning it internaly
			 *
			 * @param value the double value that will be assigned to the type
			 */
			explicit LuaTNumber(double _value) : LuaType(), value(std::move(_value)) {}

			/**
			 * @brief Default destructor
			 */
			~LuaTNumber() {}

			/**
			 * @brief Returns the type id as deifend in the `lua.h`
			 *
			 * @detail
			 * Returns the lua type id for the nil type: `LUA_TNUMBER`
			 *
			 * @see LuaType.getTypeId()
			 */
			int getTypeId() const;
			
			/**
			 * @brief Returns the string representation of the type
			 *
			 * @details
			 * Returns the string interpretation of the LUA_TNUMBER type.
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
			 * on the index has the LUA_TNUMBER type, and if not will throw an error.
			 * If the value is LUA_TNUMBER, the value will be read from the `lua` context
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
			 * @brief Returns the value of the double
			 *
			 * @details
			 * Returns the value of the double variable
			 *
			 * @return
			 * double value encapsulated by the class
			 */
			double getValue() const;

			/**
			 * @brief Set the double value
			 *
			 * @details
			 * Sets the value of the double variable
			 *
			 * @param value value that will be encapsulated
			 */
			void setValue(double value);
		};
	}
}
#endif // LUACPP_LUATNUMBER_HPP
