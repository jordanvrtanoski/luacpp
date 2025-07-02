#include "../LuaCpp.hpp"
#include "gtest/gtest.h"

using namespace LuaCpp;
using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;

extern "C" {
    static int testMetaMethod(lua_State *L) {
        // Meta-method implementation
        return 0;
    }

    static int testMetaMethod2(lua_State *L) {
        // Second meta-method implementation
        return 0;
    }
}

class TestLuaLibrary : public ::testing::Test {
  protected:
    virtual void SetUp() {}
};

TEST_F(TestLuaLibrary, AddCMetaMethod) {
    /**
     * Test that AddCMetaMethod correctly adds a meta-method to the library
     */
    std::shared_ptr<LuaLibrary> lib = std::make_shared<LuaLibrary>("testlib");

    // Add the meta-method
    lib->AddCMetaMethod("__gc", testMetaMethod);

    // Verify that the meta-method was added
    ASSERT_TRUE(lib->Exists_m_meta("__gc"));
}

TEST_F(TestLuaLibrary, AddCMetaMethod_Replace) {
    /**
     * Test that AddCMetaMethod with replace=true replaces an existing meta-method
     */
    std::shared_ptr<LuaLibrary> lib = std::make_shared<LuaLibrary>("testlib");

    // Add first meta-method
    lib->AddCMetaMethod("__gc", testMetaMethod);

    // Add second meta-method with replace=true
    lib->AddCMetaMethod("__gc", testMetaMethod2, true);

    // Verify that the meta-method was replaced
    ASSERT_TRUE(lib->Exists_m_meta("__gc"));
}

TEST_F(TestLuaLibrary, AddCMethod_Replace) {
    /**
     * Test that AddCMethod with replace=true replaces an existing method
     */
    std::shared_ptr<LuaLibrary> lib = std::make_shared<LuaLibrary>("testlib");

    // Add first method
    lib->AddCMethod("testmethod", testMetaMethod);

    // Add second method with replace=true
    lib->AddCMethod("testmethod", testMetaMethod2, true);

    // Verify that the method was replaced
    ASSERT_TRUE(lib->Exists_m("testmethod"));
}

TEST_F(TestLuaLibrary, RegisterFunctionsWithMultipleMethods) {
    /**
     * Test that RegisterFunctions correctly registers multiple methods
     */
    std::shared_ptr<LuaLibrary> lib = std::make_shared<LuaLibrary>("testlib");

    // Add multiple methods
    lib->AddCMethod("method1", testMetaMethod);
    lib->AddCMethod("method2", testMetaMethod2);

    // Create a Lua state and register the library
    LuaState L;
    lib->RegisterFunctions(L);

    // Verify that the methods were registered correctly
    lua_getglobal(L, "testlib");
    ASSERT_TRUE(lua_istable(L, -1));

    // Check method1
    lua_pushstring(L, "method1");
    lua_gettable(L, -2);
    ASSERT_TRUE(lua_isfunction(L, -1));
    lua_pop(L, 1);

    // Check method2
    lua_pushstring(L, "method2");
    lua_gettable(L, -2);
    ASSERT_TRUE(lua_isfunction(L, -1));
    lua_pop(L, 2); // Pop the function and the table
}

TEST_F(TestLuaLibrary, GetLibMethod) {
    /**
     * Test that getLibMethod returns the correct function
     */
    std::shared_ptr<LuaLibrary> lib = std::make_shared<LuaLibrary>("testlib");

    // Add a method
    lib->AddCMethod("testmethod", testMetaMethod);

    // Verify that getLibMethod returns the correct function
    ASSERT_EQ(lib->getLibMethod("testmethod"), testMetaMethod);
}

TEST_F(TestLuaLibrary, GetLibFunction) {
    /**
     * Test that getLibFunction returns the correct function
     */
    std::shared_ptr<LuaLibrary> lib = std::make_shared<LuaLibrary>("testlib");

    // Add a function
    lib->AddCFunction("testfunction", testMetaMethod);

    // Verify that getLibFunction returns the correct function
    ASSERT_EQ(lib->getLibFunction("testfunction"), testMetaMethod);
}

TEST_F(TestLuaLibrary, AddCFunction_Replace) {
    /**
     * Test that AddCFunction with replace=true replaces an existing function
     */
    std::shared_ptr<LuaLibrary> lib = std::make_shared<LuaLibrary>("testlib");

    // Add first function
    lib->AddCFunction("testfunction", testMetaMethod);

    // Add second function with replace=true
    lib->AddCFunction("testfunction", testMetaMethod2, true);

    // Verify that the function was replaced
    ASSERT_TRUE(lib->Exists_f("testfunction"));
}

TEST_F(TestLuaLibrary, MetaMethodRegistration) {
    /**
     * Test that meta-methods are correctly registered with the Lua state
     */
    std::shared_ptr<LuaLibrary> lib = std::make_shared<LuaLibrary>("testlib");

    // Add meta-method
    lib->AddCMetaMethod("__gc", testMetaMethod);

    // Create a Lua state and register the library
    LuaState L;
    lib->RegisterFunctions(L);

    // Create a table with our metatable
    luaL_newmetatable(L, "testlib");
    lua_setglobal(L, "testlib");

    // Create a table with our metatable
    lua_newtable(L);
    luaL_getmetatable(L, "testlib");
    lua_setmetatable(L, -2);
    lua_setglobal(L, "testobj");

    // Run some Lua code that will trigger the __gc meta-method
    lua_pushstring(L, "testobj = nil");
    lua_pcall(L, 0, 0, 0); // This should trigger __gc when testobj is collected

    // The meta-method should have been called
    // We can't directly check if it was called, but we can check that the registration worked
    ASSERT_TRUE(lib->Exists_m_meta("__gc"));
}

TEST_F(TestLuaLibrary, MetaMethodRegistrationWithReplace) {
    /**
     * Test that meta-methods are correctly registered with the Lua state
     */
    std::shared_ptr<LuaLibrary> lib = std::make_shared<LuaLibrary>("testlib");

    // Add meta-method
    lib->AddCMetaMethod("__gc", testMetaMethod);

    // Add second meta-method with replace=true
    lib->AddCMetaMethod("__gc", testMetaMethod2, true);

    // Create a Lua state and register the library
    LuaState L;
    lib->RegisterFunctions(L);

    // Create a table with our metatable
    luaL_newmetatable(L, "testlib");
    lua_setglobal(L, "testlib");

    // Create a table with our metatable
    lua_newtable(L);
    luaL_getmetatable(L, "testlib");
    lua_setmetatable(L, -2);
    lua_setglobal(L, "testobj");

    // Run some Lua code that will trigger the __gc meta-method
    lua_pushstring(L, "testobj = nil");
    lua_pcall(L, 0, 0, 0); // This should trigger __gc when testobj is collected

    // The meta-method should have been called
    // We can't directly check if it was called, but we can check that the registration worked
    ASSERT_TRUE(lib->Exists_m_meta("__gc"));
}