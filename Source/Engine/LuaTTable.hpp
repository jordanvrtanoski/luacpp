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


#ifndef LUACPP_LUATTABLE_HPP
#define LUACPP_LUATTABLE_HPP

#include <map>
#include <memory>

#include "../Lua.hpp"
#include "LuaState.hpp"
#include "LuaType.hpp"

namespace LuaCpp {
	namespace Engine {
		namespace Table {
			/**
			 * @brief A helper class that holds the key type
			 *
			 * @details
			 * Lua allows the tables to have mixed types: LUA_TNUMBER or LUA_TSTRING. This
			 * class allows unifications of both key types as one `C++` type for unified
			 * handling.
			 */
			class Key {
			  private:
				bool _isNumber;
				std::string str_val;
				int int_val;
			  public:
				explicit Key(int value) : _isNumber(true), str_val(), int_val(value) {}
				explicit Key(std::string value) : _isNumber(false), str_val(std::move(value)), int_val(0) {}
				explicit Key(const char *value) : _isNumber(false), str_val(std::string(value)), int_val(0) {}

				bool isNumber() const;

				std::string getStringValue() const;
				int getIntValue() const;

				std::string ToString() const;

				friend bool operator <(const Key &lhs, const Key &rhs);
				friend bool operator ==(const Key &lhs, const Key &rhs);
				friend std::ostream& operator<<(std::ostream& os, const Key &key);

			};
		}

		/**
		 * @brief Implementation of the LUA_TTABLE type
		 *
		 * @detail
		 * Implementation of LUA_TTABLE
		 */
		class LuaTTable : public LuaType {
		   private:
			/**
			 * @brief true if the table is array (only integer keys)
			 *
			 * @details
			 * If all of the keys are integers, than the table is an array.
			 */
			bool _isArray;
			/**
			 * @brief table information in the `C++` context
			 *
			 * @details
			 * The table information copied in the `C++` context.
			 */
			 std::map<Table::Key, std::shared_ptr<LuaType>> table;
		   public:

			/**
			 * @brief explicit constructor
			 *
			 * @details
			 * Explicit constructor of the table
			 */
			 explicit LuaTTable() :  LuaType(), _isArray(true), table() {}

			/**
			 * @brief Default destructor
			 */
			~LuaTTable() {}

			/**
			 * @brief Returns the type id as deifend in the `lua.h`
			 *
			 * @detail
			 * Returns the lua type id for the nil type: `LUA_TTABLE`
			 *
			 * @see LuaType.getTypeId()
			 */
			int getTypeId() const;

			/**
			 * @brief Returns the string representation of the type
			 *
			 * @details
			 * Returns the string interpretation of the LUA_TTABLE type.
			 *
			 * @see LuaType.getTypeName()
			 *
			 */
			std::string getTypeName(LuaState &L) const;

			/**
			 * @brief Pushes the table on the top fo the stack
			 *
			 * @details
			 * Pushes a table on top of the stack
			 *
			 * @see LuaType.PushValue()
			 */
			void PushValue(LuaState &L);

			/**
			 * @brief Reads the value from the stack
			 *
			 * @details
			 * Reads a vlue from the stack. The method will check if the stack
			 * on the index has the LUA_TTTABLE type, and if not will throw an error.
			 * If the value is LUA_TTABLE, the value will be read from the `lua` context 
			 *
			 * The table will be traveresed in the `lua` context and the infromation extracted from
			 * each field in the table. After the traversal, the stack will be restored in the 
			 * original state.
			 *
			 * The stack remaines balanced after the call
			 *
			 * @see LuaType.PopValue()
			 */
			using LuaType::PopValue;
			void PopValue(LuaState &L, int idx);

			/**
			 * @brief Returns the JSON string representation of the table
			 *
			 * @details
			 * Returns the keys and values from the table in a JSON format
			 */
			std::string ToString() const;

			/**
			 * @brief Returns the encapsulated std::map
			 *
			 * @details
			 * Returns the encapsulated std::map
			 *
			 * @return
			 * the encapsulated table
			 */
			std::map<Table::Key, std::shared_ptr<LuaType>> getValues() const;

			/**
			 * @breif Returns the value stored at the key
			 *
			 * @details
			 * Returns the value stored at the key. If the key is not found,
			 * a nil value will be returned
			 *
			 * @returns
			 * Valute associtated with the key
			 */
			LuaType &getValue(Table::Key key);

			/**
			 * @brief Sets a value for the associtated key
			 *
			 * @details
			 * Sets a value for the associated key
			 *
			 * @param key the key of the table
			 * @param value the associated value
			 */
			void setValue(Table::Key key, std::shared_ptr<LuaType> value);

		};
	}
}

#endif // LUACPP_LUATTABLE_HPP

