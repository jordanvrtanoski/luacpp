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

#ifndef LUACPP_LUACODESNIPPET_HPP
#define LUACPP_LUACODESNIPPET_HPP
#include <string>
#include <vector>

#include "../Lua.hpp"
#include "../Engine/LuaState.hpp"

extern "C" {
	/**
	 * @brief `C` helper function for writting binary data to the code snippet
	 *
	 * @details
	 * `C` helper function implementing `lua_Writter` interface. The funciton
	 * is used to write the binary data to the LuaCodeSnippet during the
	 * compilation.
	 *
	 * @param L Lua State (instance of a Lua virtual machine)
	 * @param p Pointer to the buffer containing the fragment
	 * @param size Size of the buffer
	 * @param u Pointer to LuaCodeSnippet that will hold the compiled code
	 *
	 * @return 0 in case of sucess, non 0 in case of error.
	 */
	int code_writer (lua_State* L, const void* p, size_t size, void* u);

	/**
	 * @brief `C` helper function for reading the binary code from the snippet
	 *
	 * @details
	 * `C` helper function that will read the code from the LuaCodeSnippet buffer
	 * and will upload it to the virtual machine. The function implements the
	 * `lua_Reader` interface.
	 *
	 * @param L Lua State (instance of a Lua virtual machine)
	 * @param data Pointer to the LuaCodeSnippet
	 * @param size Size of the buffer (returning)
	 *
	 * @return A pointer to a buffer cotainig the code snippet. 
	 */
	const char * code_reader (lua_State *L, void *data, size_t *size);
}

namespace LuaCpp {
	/**
	 * @brief Classes that are related to the management of the Lua code
	 *
	 * @details
	 * Management of the lua code snippets and libraries belonging 
	 * to a LuaContext
	 */
	namespace Registry {

		/**
		 * @brief Container Class holding the compiled Lua code
		 *
		 * @details
		 * To allow faster re-execution of the Lua code, the code
		 * is compiled in a binary form during the loading phase
		 * and storeed in the snippet. When a LuaState is created by
		 * the LuaContext for the exeuction, the binary is loaded back
		 * to the Lua instance. This saves the time for compilation
		 * of large scripts in systems where the script is re-executed
		 * multiple times.
		 */
		class LuaCodeSnippet {
			private:
				/**
				 * @brief Name of the snippet
				 *
				 * @details
				 * Name of the code snippet used to identify the code
				 * in the Lua instance
				 */
				std::string name;

				/**
				 * @brief Code buffer
				 *
				 * @details
				 * Contains the code in a binray form
				 */
				std::vector<unsigned char> code;

			public:
				/**
				 * @brief Default constructure that initializes the buffer
				 *
				 * @details
				 * Initializes the buffer for the binary code
				 */
				LuaCodeSnippet(); 

				/**
				 * @brief Default destructor
				 */
				~LuaCodeSnippet() {}

				/**
				 * @brief Writes a chunk of the binary data from the input buffer to the code buffer
				 *
				 * @details 
				 * Called by the `C` helper function `write_code` to add a chunk in the code buffer. 
				 * The lua virtual machine will call the `wrtie_code` multiple times (at least 2 times)
				 * with the chunks of the compiled code. Each call will receive the chunk size. The method
				 * aggregates the chunks by appending the new chink at the end of the code buffer.
				 *
				 * @see write_code
				 *
				 * @param buff Binary buffer containeg the code chunk
				 * @param size Size of the code chunk
				 *
				 * @return 0 for sucess, 1 otherwise
				 */
				int WriteCode(unsigned char* buff, size_t size);
				
				/**
				 * @brief Uploads the code to the instace
				 *
				 * @details
				 * Low level function that will upload the code to Lua virtual machine identified by the `lua_State`
				 * with the use of the `C` helper functions.
				 *
				 * @param L Lua state (instance of Lua virtual machine)
				 */
				void UploadCode(Engine::LuaState &L);

				/**
				 * @brief Returns the pointer to the continious memory block containing the binary code
				 *
				 * @details
				 * Returns a pointer to the continious memory block thet holds the binary representation
				 * of the Lua code.
				 */
				const char *getBuffer();

				/**
				 * @brief Returns the total size of the code buffer
				 *
				 * @details 
				 * Returns the total size of the code buffer
				 *
				 * @return
				 * Size of the code buffer
				 */
				int getSize();

				/**
				 * @brief Returns the name of the code snippet
				 *
				 * @details
				 * Returns the name of the code snuppet under whic it's uploaded in the Lua instance
				 *
				 * @return Snippet Name
				 */
				std::string getName();

				/**
				 * @brief Sets the snippet name
				 *
				 * @details
				 * Sets the snippet name
				 *
				 * @param name Snippet name
				 */
				void setName(std::string name);
		};
	}
}

#endif // LUACPP_LUACODESNIPPET_HPP
