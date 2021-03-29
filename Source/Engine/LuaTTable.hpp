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
			class Key {
			  private:
				bool _isNumber;
				std::string str_val;
				int int_val;
			  public:
				explicit Key(int value) : _isNumber(true), int_val(value), str_val() {}
				explicit Key(std::string value) : _isNumber(false), str_val(std::move(value)), int_val(0) {}
				explicit Key(const char *value) : _isNumber(false), str_val(std::string(value)), int_val(0) {}
				
				bool isNumber();

				std::string getStringValue();
				int getIntValue();

				std::string ToString();

				friend bool operator <(const Key &lhs, const Key &rhs);
				friend bool operator ==(const Key &lhs, const Key &rhs);
				friend std::ostream& operator<<(std::ostream& os, const Key &key);

			};
		}
		class LuaTTable : public LuaType {
		   private:
			bool _isArray;
	                std::map<Table::Key, std::shared_ptr<LuaType>> table;
		   public:
			LuaTTable() : table(), LuaType(), _isArray(true) {}
			~LuaTTable() {}

			int getTypeId();
			std::string getTypeName(LuaState &L);
			void PushValue(LuaState &L);
			void PopValue(LuaState &L, int idx);
			std::string ToString();

			std::map<Table::Key, std::shared_ptr<LuaType>> getValues();
			LuaType &getValue(Table::Key key);
			void setValue(Table::Key key, std::shared_ptr<LuaType> value);

		};
	}
}

#endif // LUACPP_LUATTABLE_HPP

