#include "../LuaCpp.hpp"
#include <iostream>
#include <stdexcept>

using namespace LuaCpp;
using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;

extern "C" {
    // Meta-method implementation for __gc
    static int myMetaMethod(lua_State *L) {
        // Get the userdata object from the Lua stack
        void *ud = lua_touserdata(L, 1);
        if (ud) {
            // Clean up resources if needed
            std::cout << "Cleaning up userdata" << std::endl;
            // Free the memory if it was allocated
            // free(ud);
        }
        return 0;
    }

    // Regular function to be called from Lua
    static int hello(lua_State *L) {
        std::cout << "Hello from C!" << std::endl;
        lua_pushstring(L, "Hello from C!");
        return 1;
    }
}

int main(int argc, char **argv) {
    try {
        // Create a Lua context
        LuaContext lua;

        // Create a library with meta-methods
        std::shared_ptr<LuaLibrary> lib = std::make_shared<LuaLibrary>("mylib");

        // Add a regular function to the library
        lib->AddCFunction("hello", hello);

        // Add a meta-method to the library
        lib->AddCMetaMethod("__gc", myMetaMethod);

        // Add the library to the context
        lua.AddLibrary(lib);

        // Compile and run a Lua script that uses the library
        lua.CompileString("test", "print(mylib.hello())");

        // Create a userdata object in Lua
        lua.CompileString("create_ud", "local ud = {} setmetatable(ud, {__gc = function() print('__gc called from Lua') end}) return ud");

        // Run the script
        lua.Run("test");

        // Create and immediately collect the userdata
        {
            std::unique_ptr<LuaState> L = lua.newStateFor("create_ud");
            // The userdata will be collected when the state is destroyed
        }

        std::cout << "Script executed successfully" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}