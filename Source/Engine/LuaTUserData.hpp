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


#ifndef LUACPP_LUATUSERDATA_HPP
#define LUACPP_LUATUSERDATA_HPP

#include <map>

#include "../Lua.hpp"
#include "LuaState.hpp"
#include "LuaType.hpp"

namespace LuaCpp {
	namespace Engine {
		/**
		 * @brief Implementation of the LUA_TUSERDATA as a base object
		 *
		 * @details
		 * Implementation for the LUA_TUSERDATA as a base object that can 
		 * be used to implement user types with underling MetaTable.
		 *
		 * The LUA_TUSERDATA type represents a pointer to a memory buffer in 
		 * which the user data can be stored. The associtate `metatable` contains
		 * list of metafunctions implemented in `C` that the lua engine will invoke 
		 * in order to operate with the user type. See lua documents for more 
		 * details on the supported meta functions.
		 */
		class LuaTUserData : public LuaType {
		   protected:
			/**
			 * @brief the pointer to the buffer that will hold the user data
			 */
			void *userdata;

			/** 
			 * @brief the size of the user data
			 */
			size_t size;

			/**
			 * @brief the meta table.
			 */
			std::map<std::string, lua_CFunction> metatable;

			/**
			 * @brief Store data in the buffer after allocation
			 *
			 * @details
			 * Called by the push methods after the type
			 * is create on the stack of the `LuaState` and the
			 * buffer is allocated. Objects that inherit this class
			 * should implement this method to store their data in the
			 * `lua` context
			 */
			virtual void _storeData();
		
			/**
			 * @brief Read the user data
			 *
			 * @details
			 * Called by the pop methods when the type is
			 * ready to be collected from the `lua` context
			 * in order for the data stored in the user
			 * buffer to be collecte
			 */
			virtual void _retreiveData();
		   public:

			/**
			 * @brief Explicite constructuor
			 *
			 * @details
			 * Explicit constructor that recived the
			 * size of the buffer that should be allocated to hold the
			 * user data in the `lua` context.
			 */
			explicit LuaTUserData(size_t _size) : LuaType(), userdata(NULL), size(_size), metatable() {}
			
			/**
			 * @brief Default destructor
			 */
			~LuaTUserData() {}

			/**
			 * @brief Returns the type id as deifend in the `lua.h`
			 *
			 * @detail
			 * Returns the lua type id for the nil type: `LUA_TUSERDATA`
			 *
			 * @see LuaType.getTypeId()
			 */
			int getTypeId() const;

			/**
			 * @brief Returns the string representation of the type
			 *
			 * @details
			 * Returns the string interpretation of the `LUA_TUSERDATA` type.
			 *
			 * @see LuaType.getTypeName()
			 *
			 */
			std::string getTypeName(LuaState &L) const;

			/**
			 * @brief Pushes the user data on the top fo the stack
			 *
			 * @details
			 * Pushes a user data on top of the stack. The push will 
			 * create the lua type on the stack and allocate a memory
			 * region with the specified size after which the `_storeData()`
			 * will be invoked to populate the buffer.
			 *
			 * @see LuaType.PushValue()
			 */
			void PushValue(LuaState &L);

			/**
			 * @brief Reads the value from the stack
			 *
			 * @details
			 * Reads a vlue from the stack. The method will check if the stack
			 * on the index has the LUA_TUSERDATA type, and if not will throw an error.
			 * If the value is LUA_TUSERDATA, the value will be read from the `lua` context 
			 *
			 * @see LuaType.PopValue()
			 */
			using LuaType::PopValue;
			void PopValue(LuaState &L, int idx);

			/**
			 * @brief Returns the `userdata` from the value
			 *
			 * @details
			 * Returns the string representing the user data
			 */
			virtual std::string ToString() const;

			/**
			 * @brief Returns the raw pointer to the allocated buffer
			 *
			 * @details
			 * Returns the raw pointer to the allocated buffer
			 */
			void *getRawUserData() const;

			/**
			 * @biref adds a metafunction in the metatable
			 *
			 * @details
			 * Adds a meta function in the meta table. The meta table
			 * will be create for the type after the buffer for the
			 * data storage will be allocated.
			 */
			void AddMetaFunction(std::string fname, lua_CFunction fn);

		};
	}
}
#endif // LUACPP_LUATUSERDATA_HPP


