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

#ifndef LUACPP_LUACONTEXT_HPP
#define LUACPP_LUACONTEXT_HPP

#include <memory>

#include "Registry/LuaRegistry.hpp"
#include "Registry/LuaLibrary.hpp"
#include "Engine/LuaState.hpp"
#include "Engine/LuaType.hpp"

namespace LuaCpp {
	/**
	 * @brief Lua Context for execution
	 *
	 * @details
	 * LuaContext represents a enclosed execution context
	 * which holds the compiled code snippets and global variables
	 *
	 */

	typedef std::map<std::string, std::shared_ptr<Engine::LuaType>> LuaEnvironment;

	class LuaContext {
		/**
		 * @brief Name of the context
		 */
		std::string name;

		/**
		 * @brief Repository that will keep the code snippets
		 * 
		 * @see LuaContext::Registry::LuaRegistry
		 */
		Registry::LuaRegistry registry;

		/**
		 * Custom `C` librraries for the session
		 */
		std::map<std::string, std::shared_ptr<Registry::LuaLibrary>> libraries;

		/**
		 *
		 */
		LuaEnvironment globalEnvironment;

	public:

		/**
		 * @brief Constructs and empty context
		 *
		 * @details
		 * Creates empty Lua cotext. This is the main entry point
		 * for the communication with the Lua virtual machine
		 * from the high level APIs.
		 */
		LuaContext() : registry(), libraries(), globalEnvironment() {};
		~LuaContext() {};

		/**
		 * @brief Creates new Lua execution state from the context
		 *
		 * @details
		 * Creates new Lua execution state from the context. The
		 * state will be loaded with the standard libraries 
		 * and the libraries that are registered in the context
		 *
		 * The globalEnvironment variables will also be loaded to the context.
		 *
		 * @return Pointer to the LuaState object holding the pointer of the lua_State
		 */
		std::unique_ptr<Engine::LuaState> newState();

		/**
		 * @brief Creates new Lua execution state from the context
		 *
		 * @details
		 * Creates new Lua execution state from the context. The
		 * state will be loaded with the standard libraries 
		 * and the libraries that are registered in the context
		 *
		 * The provided environment will be loaded instead of the 
		 * global environment.
		 *
		 * @return Pointer to the LuaState object holding the pointer of the lua_State
		 */
		std::unique_ptr<Engine::LuaState> newState(const LuaEnvironment &env);

		/**
		 * @brief Creates new Lua execution state from the context and loads a snippet
		 *
		 * @details
		 * Creates new Lua execution state from the context. The
		 * state will be loaded with the standard libraries 
		 * and the libraries that are registered in the context. In addition to the
		 * libraries, the context will be loaded with the selected snippet.
		 * The snippet will be on top of the lua stack (position 1) and is ready
		 * for execution, or definition of a global name
		 *
		 * The variables defined in the globalEnvironment will be loaded on the
		 * state.
		 *
		 * If the name is not found, the method will throw exception
		 *
		 * @param name Name of the snippet to be loaded
		 *
		 * @return Pointer to the LuaState object holding the pointer of the lua_State
		 */
	        std::unique_ptr<Engine::LuaState> newStateFor(const std::string &name);
		
		/**
		 * @brief Creates new Lua execution state from the context and loads a snippet
		 *
		 * @details
		 * Creates new Lua execution state from the context. The
		 * state will be loaded with the standard libraries 
		 * and the libraries that are registered in the context. In addition to the
		 * libraries, the context will be loaded with the selected snippet.
		 * The snippet will be on top of the lua stack (position 1) and is ready
		 * for execution, or definition of a global name
		 *
		 * The varibles provided with the `LuaEnvironment` will also be loaded on
		 * the state as a global variables.
		 *
		 * If the name is not found, the method will throw exception
		 *
		 * @param name Name of the snippet to be loaded
		 *
		 * @return Pointer to the LuaState object holding the pointer of the lua_State
		 */
	        std::unique_ptr<Engine::LuaState> newStateFor(const std::string &name, const LuaEnvironment &env);

		/**
		 * @brief Compiles a string containing Lua code and adds it to the repository
		 *
		 * @details
		 * Compiles a string containing Lua code and adds the compiled binary to the
		 * repository as a LuaCodeSnippet. The code is registered in Lua engine and in 
		 * the repository under the same name. If the compilation fails, the function
		 * will throw an `std::logic_error` with the error code received from Lua engin
		 *
		 * @param name Name under which the snippet is registered in the repository
		 * @param code A valid Lua code that will be compiled
		 */
		void CompileString(const std::string &name, const std::string &code);
		
		/**
		 * @brief Compiles a string containing Lua code and adds it to the repository
		 *
		 * @details
		 * Compiles a string containing Lua code and adds the compiled binary to the
		 * repository as a LuaCodeSnippet. The code is registered in Lua engine and in 
		 * the repository under the same name. If the compilation fails, the function
		 * will throw an `std::logic_error` with the error code received from Lua engin
		 *
		 * @param name Name under which the snippet is registered in the repository
		 * @param code A valid Lua code that will be compiled
		 * @param recompile if true, the new version of the code will be active
		 */
		void CompileString(const std::string &name, const std::string &code, bool recompile);

		/**
		 * @brief Compiles a fle containing Lua code and adds it to the registry
		 *
		 * @details
		 * Compiles a file containing Lua code and adds the compiled binary to the
		 * repository as a LuaCodeSnippet. The code is registered in Lua engine and in 
		 * the repository under the same name. If the compilation fails, the function
		 * will throw an `std::logic_error` with the error code received from Lua engin
		 *
		 * @param name Name under which the snippet is registered in the registry
		 * @param code path to the file where the code is stored
		 */
		void CompileFile(const std::string &name, const std::string &fname);

		/**
		 * @brief Compiles a fle containing Lua code and adds it to the registry
		 *
		 * @details
		 * Compiles a file containing Lua code and adds the compiled binary to the
		 * repository as a LuaCodeSnippet. The code is registered in Lua engine and in 
		 * the repository under the same name. If the compilation fails, the function
		 * will throw an `std::logic_error` with the error code received from Lua engin
		 *
		 * @param name Name under which the snippet is registered in the registry
		 * @param code path to the file where the code is stored
		 * @param recompile if set to true, the new code will replace the old in the registry
		 */
		void CompileFile(const std::string &name, const std::string &fname, bool recompile);


		/**
		 * @brief Compiles all of the `.lua` files from the folder and adds them to the registry
		 *
		 * @details
		 * Compiles the files with `.lua` extension and adds them to the
		 * repository as a LuaCodeSnippet. The code is registered in Lua engine and in 
		 * the repository under the name of the file without the extenaion. If the compilation fails, 
		 * for certain file, the file will not be added to the repository. No execption will be
		 * thrown for files containing syntax errro, and the files will not be added to the registry.
		 *
		 * Ex. If the folder contains the files:
		 * ```
		 *   lua_files
		 *      |- file1.lua
		 *      |- file2.lua
		 *      |- file3
		 *      |- file4.lua.old
		 *      |- folder
		 *          |- file5.lua
		 * ```
		 * The registry will contain only the `file1` and `file2` entries. The `file3` will be
		 * skipped because there is missing extension, `file4` beacuse the extension is `.old`
		 * and `file5` because is in a sub-folder which is not scaned.
		 *
		 * If the snippet already exists under the same name in the registry, the file
		 * will be skipped (not added to the registry).
		 *
		 * @param path Path to the folder containing the `.lua` files
		 */
		void CompileFolder(const std::string &path);

		/**
		 * @brief Compiles all of the `.lua` files from the folder and adds them to the registry
		 *
		 * @details
		 * Compiles the files with `.lua` extension and adds them to the
		 * repository as a LuaCodeSnippet. The code is registered in Lua engine and in 
		 * the repository under the file name prepended with the prefix and `.` (see examples below). 
		 * The file name of the file will be without the extenaion. If the compilation fails, 
		 * for certain file, the file will not be added to the repository. No execption will be
		 * thrown for files containing syntax errro, and the files will not be added to the registry.
		 *
		 *
		 * Ex. If the folder contains the filesi, and the prefix is `local`:
		 * ```
		 *   lua_files
		 *      |- file1.lua
		 *      |- file2.lua
		 *      |- file3
		 *      |- file4.lua.old
		 *      |- folder
		 *          |- file5.lua
		 * ```
		 * The registry will contain only the `local.file1` and `local.file2` entries. The `file3` will be
		 * skipped because there is missing extension, `file4` beacuse the extension is `.old`
		 * and `file5` because is in a sub-folder which is not scaned.
		 *
		 * If the snippet already exists under the same name in the registry, the file
		 * will be skipped (not added to the registry).
		 *
		 * @param path Path to the folder containing the `.lua` files
		 * @param prefix The prefix appended to the path.
		 */
		void CompileFolder(const std::string &path, const std::string &prfix);
		
		/**
		 * @brief Compiles all of the `.lua` files from the folder and adds them to the registry
		 *
		 * @details
		 * Compiles the files with `.lua` extension and adds them to the
		 * repository as a LuaCodeSnippet. The code is registered in Lua engine and in 
		 * the repository under the file name prepended with the prefix and `.` (see examples below). 
		 * The file name of the file will be without the extenaion. If the compilation fails, 
		 * for certain file, the file will not be added to the repository. No execption will be
		 * thrown for files containing syntax errro, and the files will not be added to the registry.
		 *
		 *
		 * Ex. If the folder contains the filesi, and the prefix is `local`:
		 * ```
		 *   lua_files
		 *      |- file1.lua
		 *      |- file2.lua
		 *      |- file3
		 *      |- file4.lua.old
		 *      |- folder
		 *          |- file5.lua
		 * ```
		 * The registry will contain only the `local.file1` and `local.file2` entries. The `file3` will be
		 * skipped because there is missing extension, `file4` beacuse the extension is `.old`
		 * and `file5` because is in a sub-folder which is not scaned.
		 *
		 * If the snippet already exists under the same name in the registry, and `recompile` is set to true,
		 * the onl snippet will be replaced with the file, otherwise, if the `recompile` is false, the file 
		 * will be skipped (not added to the registry).
		 *
		 * @param path Path to the folder containing the `.lua` files
		 * @param prefix The prefix appended to the path.
		 * @param recompile If true, the file will be added to registry even if it already exits under the name.
		 */
		void CompileFolder(const std::string &path, const std::string &prefix, bool recompile);
		
		/**
		 * @bried Compiles a code snippet and runs
		 *
		 * @details
		 * The code sneippet will be compiled under the name `default` and
		 * will be executed in the context.
		 *
		 * This is the highest level function and is ment for prototyping
		 * use since the code is recompiled with each call. For use in
		 * project, use `Run` instead
		 *
		 * @see Run(std::string name)
		 *
		 * @param code Code to run
		 */
		void CompileStringAndRun(const std::string &code);

		/**
		 * @bried Compiles the file and runs
		 *
		 * @details
		 * The file will be compiled under the name `default` and
		 * will be executed in the context.
		 *
		 * This is the highest level function and is ment for prototyping
		 * use since the code is recompiled with each call. For use in
		 * project, use `Run` instead
		 *
		 * @see Run(std::string name)
		 *
		 * @param code Code to run
		 */
		void CompileFileAndRun(const std::string &code);
		
		/**
		 * @bried Run a code snippet
		 *
		 * @details
		 * Run a snippet that was previously compiled and stored in the registry
		 *
		 * @param name Name under which the snippet is registered
		 */
		void Run(const std::string &name);

		/**
		 * @bried Run a code snippet with a given `lua` global table
		 *
		 * @details
		 * Run a snippet that was previously compiled and stored in the registry
		 * with a given environment (lua global variables)
		 *
		 * @param name Name under which the snippet is registered
		 */
		void RunWithEnvironment(const std::string &name, const LuaEnvironment &env);

		/**
		* @brief Add a `C` library to the context
		*
		* @details
		* Adds a `C` library to the context. The library will be loaded whenever a
		* new state is created fron the context.
		*
		* @param library The library containing `C` functions
		*/
		void AddLibrary(std::shared_ptr<Registry::LuaLibrary> &library);

		/**
		 * @brief Add a global variable
		 *
		 * @details
		 * Add global variable. The variables will be populated in
		 * the LuaState before the state is returned from the 
		 * newState() and newStateFor() methods.
		 *
		 * The variables will be accessible from the lua enging
		 * under the registered name.
		 *
		 * @param name name of the global variable
		 * @param var the variable
		 */
		void AddGlobalVariable(const std::string &name, std::shared_ptr<Engine::LuaType> var);

		/**
		 * @brief Retrurns the shared pointer to a global variable
		 *
		 * @details
		 * Returns a shared pointer to the global variable. The variable
		 * should be reinterpreted as the proper type.
		 *
		 * @param name Name of the global variable
		 *
		 * @returns
		 * The shared pointer of the global variable
		 */
		std::shared_ptr<Engine::LuaType> &getGlobalVariable(const std::string &name);
	};
}

/**
 * @example example_helloworld.cpp
 */
#endif // LUACPP_LUACONTEXT_HPP
