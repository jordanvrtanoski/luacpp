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


#ifndef LUACPP_LUAMETAOBJECT_HPP
#define LUACPP_LUAMETAOBJECT_HPP

#include <memory>

#include "Lua.hpp"
#include "Engine/LuaState.hpp"
#include "Engine/LuaTUserData.hpp"

namespace LuaCpp {

	extern "C" {
		/**
		 * @brief `C` bridge function called the `__newindex` metafunction
		 *
		 * @details
		 * A `C` bridge function that will be used as the `__newindex` metafunction
		 * for the object. Whenver a `object["key"] = value` is invoked in the 
		 * `lua` context, the functions will be exectued. The function will
		 * identify the object associated with the user data and invoke the `_setValue()`
		 */
		static int u_newindex(lua_State *L);
		
		/**
		 * @brief `C` bridge function called the `__index` metafunction
		 *
		 * @details
		 * A `C` bridge function that will be used as the `__index` metafunction
		 * for the object. Whenver a `value = object["key"]` is invoked in the 
		 * `lua` context, the functions will be exectued. The function will
		 * identify the object associated with the user data and invoke the `_getValue()`
		 */
		static int u_index(lua_State *L);
		
		/**
		 * @brief `C` bridge function called the `__call` metafunction
		 *
		 * @details
		 * A `C` bridge function that will be used as the `__call` metafunction
		 * for the object. Whenver a `object(arguments)` is invoked in the 
		 * `lua` context, the functions will be exectued. The function will
		 * identify the object associated with the user data and invoke the `Execute()`
		 */
		static int u_call(lua_State *L);
	}

	/**
	 * @brief Base class for instrumentation of `C++` classes
	 *
	 * @details
	 * Base class used to instrument the `C++` classes for use in
	 * the `lua` context. The class is derivation of the `LuaTUserData`
	 */
	class LuaMetaObject : public Engine::LuaTUserData {
		friend int u_newindex(lua_State *L);
		friend int u_index(lua_State *L);
		friend int u_call(lua_State *L);

	   protected:
		/**
		 * @biref store the `this` pointer in the user buffer
		 *
		 * @details
		 * Stores the `this` pointer in the user buffer. The pointer
		 * will be used by the `C` bridge functions to identify the object
		 * and call the assiciated meta methods
		 */
		void _storeData();
		
		/**
		 * @brief 
		 *
		 * @details
		 */
		void _retreiveData();

		/**
		 * @brief invoked by the `C` metafunction to get the user value
		 *
		 * @details
		 * Called by the `C` function to get the user value associated with
		 * a key. During the call, the top of the stack will contain the
		 * value of the `key` and the second from the top will have the 
		 * userdata value.
		 *
		 * `lua` expects this call to place the value on the top of the stack
		 * and return `1` as the number of return values
		 *
		 * The standard implementation will invoke the `getValue()` with the
		 * key (`int` or `std::string`)
		 */
		virtual int _getValue(Engine::LuaState &L);
		
		/**
		 * @brief invoked by the `C` metafunction to set the user value
		 *
		 * @details
		 * Called by the `C` function to set the user value associated with
		 * a key. During the call, the top of the stack will contain the
		 * `value`, the next argument on the stack will be the `key` and 
		 * the third from the top will have the  userdata value.
		 *
		 * The standard implementation will invoke the `setValue()` with 
		 * the `key` (`int` or `std::string`) and the LuaType value.
		 */
		virtual int _setValue(Engine::LuaState &L);

	   public:
		LuaMetaObject(); 
		
		/** 
		 * @brief get the value of an integer key
		 *
		 * @details
		 * Get the value of the integer key
		 *
		 * @param key integer key
		 */
		virtual std::shared_ptr<LuaType> getValue(int key);
		
		/** 
		 * @brief get the value of an std::string key
		 *
		 * @details
		 * Get the value of the std::string key
		 *
		 * @param key std::string key
		 */
		virtual std::shared_ptr<LuaType> getValue(std::string &key);

		/** 
		 * @brief set the value of an integer key
		 *
		 * @details
		 * Set the value of the integer key
		 *
		 * @param key integer key
		 * @param val LuaType value
		 */
		virtual void setValue(int key, std::shared_ptr<LuaType> val);
		
		/** 
		 * @brief set the value of an std::string key
		 *
		 * @details
		 * Set the value of the std::string key
		 *
		 * @param key integer key
		 * @param val LuaType value
		 */
		virtual void setValue(std::string &key, std::shared_ptr<LuaType> val);

		/**
		 * @brief Execute the MetaObject
		 *
		 * @details
		 * Execute call to the UserData type. 
		 * The LuaState stack will contain the arguments from the call, where the top will
		 * be the last argument and the next to the bottom of the stack will be the first argument
		 * 
		 * The function should place the return values on the stack and return the number of the 
		 * return values that ware placed on the stack.
		 */
		virtual int Execute(Engine::LuaState &L);
	};
}

#endif // LUACPP_LUAMETAOBJECT_HPP
