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

#include <iostream>
#include <sstream>

#include "LuaTTable.hpp"
#include "LuaTString.hpp"
#include "LuaTNumber.hpp"
#include "LuaTBoolean.hpp"

using namespace LuaCpp::Engine;
using namespace LuaCpp::Engine::Table;

namespace LuaCpp {
	namespace Engine {
		namespace Table {
			bool operator <(const Key &lhs, const Key &rhs){
				if (lhs._isNumber and rhs._isNumber) {
					return lhs.int_val < rhs.int_val;
				}
				if (!lhs._isNumber and !rhs._isNumber) {
					return lhs.str_val < rhs.str_val;
				}
				if (lhs._isNumber) {
					return true;
				}
				return false;
			}

			bool operator ==(const Key &lhs, const Key &rhs){
				if (lhs._isNumber != rhs._isNumber) {
					return false;
				}
				if (lhs._isNumber) {
					return lhs.int_val == rhs.int_val;
				}
				return lhs.str_val == rhs.str_val;
			}
			std::ostream& operator<<(std::ostream &os, const Key &key) {
				if (key._isNumber) {
					os << std::to_string(key.int_val);
				} else {
					os << key.str_val;
				}
				return os;
			}
		}
	}
}

std::string Key::ToString() const {
	if (_isNumber) {
		return std::to_string(int_val);
	} else {
		return str_val;
	}
}

std::string Key::getStringValue() const {
	return str_val;
}

int Key::getIntValue() const {
	return int_val;
}

inline bool Key::isNumber() const {
	return _isNumber;
}


/*
 * LuaTTable
 */

int LuaTTable::getTypeId() const {
	return LUA_TTABLE;
}

std::string LuaTTable::getTypeName(LuaState &L) const {
	return std::string(lua_typename(L, LUA_TTABLE));
}

void LuaTTable::PushValue(LuaState &L) {
	lua_newtable(L);

	for (const auto &pair : table) {
		Table::Key &key = *((Table::Key *) &pair.first);
		pair.second->PushValue(L);
		if (key.isNumber()) {
			lua_seti(L, -2, key.getIntValue());
		} else {
			lua_setfield(L, -2, key.getStringValue().c_str());
		}
	}

}

void LuaTTable::PopValue(LuaState &L, int idx) {
	if (idx < 0) {
		idx = lua_gettop(L) + idx + 1; // Convert to absolute stack position by deducting the negative index from the top position
	}
	if (idx <= 0) {
		throw std::invalid_argument("The stack position " + std::to_string(idx) +" is invalid.");
	}
	if (lua_istable(L, idx) == 1) {
		table.clear();
		lua_pushnil(L);  // Push null value to the stack so the lua_next will start from the first key in the table
		while (lua_next(L, idx) != 0) {
			std::shared_ptr<LuaType> field;
			switch (lua_type(L, -1)) {
			    case LUA_TSTRING: {
				field = std::make_shared<LuaTString>("");
				field->PopValue(L, -1);
				break;
			    }  
			    case LUA_TTABLE: {
			        field = std::make_shared<LuaTTable>();
				field->PopValue(L, -1);
				break;
			    }
			    case LUA_TNUMBER: {
			        field = std::make_shared<LuaTNumber>(0);
				field->PopValue(L, -1);
				break;
		            }
			    case LUA_TBOOLEAN: {
			        field = std::make_shared<LuaTBoolean>(false);
				field->PopValue(L, -1);
				break;
			    }
			    default: {
		                field = std::make_shared<LuaTString>(lua_typename(L, lua_type(L, -1)));
				break;
		            }
			}
			if (lua_type(L, -2) == LUA_TSTRING) {
				setValue(Table::Key(lua_tostring(L,-2)), field);
			} else {
				setValue(Table::Key(lua_tointeger(L,-2)), field);
			}
			lua_pop(L,1); // Remove the value from the stack so lua_next can continue
		}
	} else {
		throw std::invalid_argument("The value at the index " + std::to_string(idx) +" is not a LUA_TTABLE");
	}
}

std::string LuaTTable::ToString() const {
	std::stringstream sso;
	if (_isArray) {
		sso << "[ ";
	} else {
		sso << "{ ";
	}
	bool add_comma = false;
	for (const auto pair: table) {
		if (add_comma) {
			sso << ", ";
		} else {
			add_comma = true;
		}
		if (!_isArray) {
			sso << "\"" << pair.first << "\" : ";
		}
		if (pair.second->getTypeId() == LUA_TSTRING) {
			sso << "\"" << pair.second->ToString() << "\"";
		} else {
			sso << pair.second->ToString();
		}
	}
	if (_isArray) {
		sso << " ]";
	} else {
		sso << " }";
	}
	return sso.str();
}

std::map<Table::Key, std::shared_ptr<LuaType>> LuaTTable::getValues() const {
	return table;
}

LuaType &LuaTTable::getValue(Table::Key key) {
	return *table[key];
}

void LuaTTable::setValue(Table::Key key, std::shared_ptr<LuaType> value) {
	if (!key.isNumber()) {
		_isArray = false;
	}
	table[key] = std::move(value);
}

