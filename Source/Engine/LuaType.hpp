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
		/**
		 * @brief Abstract base for the data types defined by the lua engine
		 *
		 * @detail
		 * Abstract base for the lua data types. The base is providing the generics
		 * required to bridge the `C/C++` types with the `lua` types.
		 *
		 */
		class LuaType {
		   private:
			/**
			 * @brief true if the type is part of the lua engine global context
			 */
			bool global;
			/**
			 * @brief global name for the type under which it will be moved to the lua engine global context
			 *
			 * @details
			 * If the `global` is true, the type will be moved from `C/C++` context to the lua global context
			 * under the specifed name. The name is set by the `PushGlobal` or `PopGlobal` methods.
			 */
			std::string globalName;

		   public:
			/**
			 * @brief Constructor of the base class
			 *
			 * @details
			 * Used to initalize the states of the `global` and `globalName` members.
			 */
			LuaType();

			/**
			 * @brief Default destructor
			 *
			 * @details
			 * Default destructor.
			 */
			virtual ~LuaType() = 0;

			/**
			 * @brief Returns the type id as deifend in the `lua.h`
			 *
			 * @details
			 * Returns the lua type id for the instance. The lua type id should be
			 * alligned with the definitions in the `lua.h`.
			 *
			 * The child class should implement this method to return the lua besic type
			 * that the class is implementing
			 *
			 * @returns
			 * integer value for the type id
			 */
			virtual int getTypeId() const = 0;

			/**
			 * @brief Returns the string representation of the type
			 *
			 * @details
			 * Returns the string representation of the type which is fetched from the 
			 * lua engine.
			 *
			 * The child class should implement this method to return the name of the
			 * type.
			 *
			 * @returns
			 * String with the name of the lua type
			 */
			virtual std::string getTypeName(LuaState &L) const = 0;

			/**
			 * @brief Pushes the value on the LuaState on the top of the stack
			 *
			 * @details
			 * Pushes the value of the instance at the top of the lua stack in the 
			 * LuaState.
			 *
			 * The child class should implement this method. After the call the
			 * final value of the type should be on top of the lua stack. Only one
			 * value should be added to the stack. There is no check if more than
			 * one value is left on the stack, however leaving the unbalanced stack
			 * can create issues with the engine. The method is used to move the 
			 * value from the `C/C++` context to the `lua` context.
			 *
			 * After the call the stack is +1
			 *
			 * @param L the LuaState representing the instance of the lua engine
			 */
			virtual void PushValue(LuaState &L) = 0;

			/**
			 * @brief Reads the value of the typy instance form an index in the stack
			 *
			 * @details
			 * Reads the value from the index position on the stack. The index can be absolute
			 * (expressed with a positive integer) or relative from the top (expressed with negative value).
			 *
			 * The stack will remain unchanged after the call.
			 *
			 * The child should implement this method to move the value of it's type
			 * from the `lua` context to the `C/C++` context. The child can use the stack
			 * during the movement of the values (ex. in case of Table), however, the stack should
			 * remain unchanged after the method returns from the call.
			 *
			 * @param L LuaState representing the instance of the engine
			 * @param idx the relative or absolute position of the variable on the stack
			 */
			virtual void PopValue(LuaState &L, int idx) = 0;

			/**
			 * @brief Provides a string representation of the value
			 *
			 * @details
			 * String representation of the value.
			 *
			 * @returns
			 * String representation of the value
			 */
			virtual std::string ToString() const = 0;

			/**
			 * @brief Reads the value from the top of the stack
			 *
			 * @details
			 * Reads the value from the top of the stack (`index = -1`).
			 *
			 * The stack will remain unchanged after the call.
			 *
			 * The child should implement this method to move the value of it's type
			 * from the `lua` context to the `C/C++` context. The child can use the stack
			 * during the movement of the values (ex. in case of Table), however, the stack should
			 * remain unchanged after the method returns from the call.
			 *
			 * @param L LuaState representing the instance of the engine
			 */
			void PopValue(LuaState &L);

			/**
			 * @brief pushes the value on the top of the stack, than pops it and registers it with global name
			 *
			 * @details
			 * Pushes the value on the stack, registeres it withing the global table associated with the LuaState
			 * and removes the value from the stack.
			 *
			 * After the call, the stack remains balanced.
			 *
			 * @param L LuaState representing the instace of the lua engine
			 * @param global_name the name under which the value will be registered
			 */
			void PushGlobal(LuaState &L, std::string global_name);

			/**
			 * @brief Reads the value from the global variable
			 *
			 * @details
			 * Reads the variable identified by the `globalName` from the `lua` engine (`LuaState`)
			 * and moves it to the internal value. The implementation is using the LuaState stack to
			 * move the variable from the global table on the stack and calls `LuaType::PopValue()` to
			 * read the value from the `lua` context to `C/C++` context. After the value is read, the
			 * variable is removed from the stack.
			 *
			 * The stack remains balanced after the call
			 *
			 * @param L LuaState in from which the global variable will be read.
			 */
			void PopGlobal(LuaState &L);

			/**
			 * @brief Reads the value from the global variable identiefed by the name
			 *
			 * @details
			 * Sets the internal `globalName` to the name provided in the call and calls the 
			 * `PopGlobal(LuaState &L)` to read the value.
			 *
			 * The stack remains balanced after the call
			 *
			 * @param L LuaState in from which the global variable will be read.
			 * @param global_name name of the variable in the `lua` context
			 */
			void PopGlobal(LuaState &L, std::string global_name);

			/**
			 * @brief ture if the instance is a global variable in a `lua` context
			 *
			 * @details
			 * Returns ture if the variable is global
			 *
			 * @returns
			 * true if the variable is global
			 */
			bool isGlobal() const;

			/**
			 * @brief returns the global name
			 *
			 * @details
			 * Returns the name of the variable in the `lua` global context
			 *
			 * @returns
			 * The global name.
			 */
			std::string getGlobalName() const;
		};
	}
}

#endif // LUACPP_LUATYPE_HPP
