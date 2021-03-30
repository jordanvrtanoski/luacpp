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

#include <fstream>

#include "../LuaCpp.hpp"
#include "gtest/gtest.h"

using namespace LuaCpp::Engine;

extern "C" {
   static int _foo (lua_State *L, int start) {
     int n = lua_gettop(L);    /* number of arguments */
     lua_Number sum = 0.0;
     int i;
     for (i = start; i <= n; i++) {
/*
       if (!lua_isnumber(L, i)) {
         lua_pushliteral(L, "incorrect argument");
         lua_error(L);
       }
*/
       sum += lua_tonumber(L, i);
     }
     lua_pushnumber(L, sum/n);        /* first result */
     lua_pushnumber(L, sum);         /* second result */
     return 2;                   /* number of results */
   }
   
   static int foo_meta (lua_State *L) {
     return _foo(L, 2);
   }
}

namespace LuaCpp {

	class TestLuaTypes : public ::testing::Test {
	  protected:
		virtual void SetUp() {
			std::ofstream of("TestLuaTypes_1_ok.lua", std::ofstream::out | std::ofstream::trunc );
			of << "sucess = true" << "\n";
			of << "if not (t['string'] == '1') then print('string is not 1') sucess = false end" << "\n";
			of << "if not (t['number'] == 5.3) then print('number is not 5.3') sucess = false end" << "\n";
			of << "if not (t['boolean']) then print('boolean is not true') sucess = false end" << "\n";
			of << "if not (t['table']['inner-1'] == '1') then print('t[inner-1] is not 1') sucess = false end" << "\n";
			of << "t['print'] = print" << "\n";
			of << "if (sucess) then print('pass')  end" << "\n";


			of.close();
		}
	};

	TEST_F(TestLuaTypes, GetValidLuaContext) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		EXPECT_NE((LuaState *) NULL, L.get());
		EXPECT_EQ(0, lua_gettop(*L));
		EXPECT_NE((const char *) NULL, lua_pushstring(*L, "test"));
		EXPECT_EQ(1, lua_gettop(*L));
	}

	TEST_F(TestLuaTypes, TestLuaTNil) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		LuaTNil nil;

		// Assure the types are set correctly
		EXPECT_EQ(LUA_TNIL, nil.getTypeId());
		EXPECT_EQ("nil", nil.getTypeName(*L));

		// Assure the state is prepared for testing
		EXPECT_NE((LuaState *) NULL, L.get());
		EXPECT_EQ(0, lua_gettop(*L));

		// Assure pushing the value
		EXPECT_NO_THROW(nil.PushValue(*L));
		EXPECT_EQ(1, lua_gettop(*L));
		EXPECT_EQ(LUA_TNIL, lua_type(*L, -1));

		// Assure popping the value in relative referencing
		EXPECT_NO_THROW(nil.PopValue(*L, -1));

		// Assure pooping the value with absolute referencing
		EXPECT_NO_THROW(nil.PopValue(*L, 1));

		// Push new value on stacks
		EXPECT_NE((const char *) NULL, lua_pushstring(*L, "test"));

		// Assure popping the value in relative referencing
		EXPECT_THROW(nil.PopValue(*L, -1), std::invalid_argument);

		// Assure pooping the value with absolute referencing
		EXPECT_THROW(nil.PopValue(*L, 2), std::invalid_argument);
		
		// Test the value
		EXPECT_EQ("nil",nil.ToString());
	}

	TEST_F(TestLuaTypes, TestLuaTString) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		LuaTString str("test_string");

		// Assure the types are set correctly
		EXPECT_EQ(LUA_TSTRING, str.getTypeId());
		EXPECT_EQ("string", str.getTypeName(*L));

		// Assure the state is prepared for testing
		EXPECT_NE((LuaState *) NULL, L.get());
		EXPECT_EQ(0, lua_gettop(*L));

		// Assure pushing the value
		EXPECT_NO_THROW(str.PushValue(*L));
		EXPECT_EQ(1, lua_gettop(*L));
		EXPECT_EQ(LUA_TSTRING, lua_type(*L, -1));

		// Assure popping the value in relative referencing
		EXPECT_NO_THROW(str.PopValue(*L, -1));
		EXPECT_EQ("test_string",str.getValue());

		// Assure pooping the value with absolute referencing
		EXPECT_NO_THROW(str.PopValue(*L, 1));
		EXPECT_EQ("test_string",str.getValue());

		// Assure change of value
		EXPECT_NO_THROW(str.setValue("test_string_2"));
		lua_pop(*L,1);
		EXPECT_NO_THROW(str.PushValue(*L));
		str.setValue("this will be changed");
		EXPECT_NO_THROW(str.PopValue(*L, 1));
		EXPECT_EQ("test_string_2",str.getValue());

		// Push new value on stacks
		lua_pushnil(*L);

		// Assure popping the value in relative referencing
		EXPECT_THROW(str.PopValue(*L, -1), std::invalid_argument);

		// Assure pooping the value with absolute referencing
		EXPECT_THROW(str.PopValue(*L, 2), std::invalid_argument);
		
		// Test the value
		EXPECT_EQ("test_string_2",str.ToString());
	}

	TEST_F(TestLuaTypes, TestLuaTNumber) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		LuaTNumber num(5.3);

		// Assure the types are set correctly
		EXPECT_EQ(LUA_TNUMBER, num.getTypeId());
		EXPECT_EQ("number", num.getTypeName(*L));

		// Assure the state is prepared for testing
		EXPECT_NE((LuaState *) NULL, L.get());
		EXPECT_EQ(0, lua_gettop(*L));

		// Assure pushing the value
		EXPECT_NO_THROW(num.PushValue(*L));
		EXPECT_EQ(1, lua_gettop(*L));
		EXPECT_EQ(LUA_TNUMBER, lua_type(*L, -1));

		// Assure popping the value in relative referencing
		EXPECT_NO_THROW(num.PopValue(*L, -1));
		EXPECT_DOUBLE_EQ(5.3,num.getValue());

		// Assure pooping the value with absolute referencing
		EXPECT_NO_THROW(num.PopValue(*L, 1));
		EXPECT_DOUBLE_EQ(5.3,num.getValue());

		// Assure change of value
		EXPECT_NO_THROW(num.setValue(5.4));
		lua_pop(*L,1);
		EXPECT_NO_THROW(num.PushValue(*L));
		num.setValue(123.123);
		EXPECT_NO_THROW(num.PopValue(*L, 1));
		EXPECT_DOUBLE_EQ(5.4,num.getValue());

		// Push new value on stacks
		lua_pushnil(*L);

		// Assure popping the value in relative referencing
		EXPECT_THROW(num.PopValue(*L, -1), std::invalid_argument);

		// Assure pooping the value with absolute referencing
		EXPECT_THROW(num.PopValue(*L, 2), std::invalid_argument);
		
		// Test the value
		EXPECT_EQ("5.400000",num.ToString());
	}

	TEST_F(TestLuaTypes, TestLuaTBoolean) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		LuaTBoolean bol(true);

		// Assure the types are set correctly
		EXPECT_EQ(LUA_TBOOLEAN, bol.getTypeId());
		EXPECT_EQ("boolean", bol.getTypeName(*L));

		// Assure the state is prepared for testing
		EXPECT_NE((LuaState *) NULL, L.get());
		EXPECT_EQ(0, lua_gettop(*L));

		// Assure pushing the value
		EXPECT_NO_THROW(bol.PushValue(*L));
		EXPECT_EQ(1, lua_gettop(*L));
		EXPECT_EQ(LUA_TBOOLEAN, lua_type(*L, -1));

		// Assure popping the value in relative referencing
		EXPECT_NO_THROW(bol.PopValue(*L, -1));
		EXPECT_EQ(true, bol.getValue());

		// Assure pooping the value with absolute referencing
		EXPECT_NO_THROW(bol.PopValue(*L, 1));
		EXPECT_EQ(true, bol.getValue());

		// Assure change of value
		EXPECT_NO_THROW(bol.setValue(false));
		lua_pop(*L,1);
		EXPECT_NO_THROW(bol.PushValue(*L));
		bol.setValue(true);
		EXPECT_NO_THROW(bol.PopValue(*L, 1));
		EXPECT_EQ(false,bol.getValue());

		// Push new value on stacks
		lua_pushnil(*L);

		// Assure popping the value in relative referencing
		EXPECT_THROW(bol.PopValue(*L, -1), std::invalid_argument);

		// Assure pooping the value with absolute referencing
		EXPECT_THROW(bol.PopValue(*L, 2), std::invalid_argument);
		
		// Test the value
		EXPECT_EQ("false",bol.ToString());
		bol.setValue(true);
		EXPECT_EQ("true",bol.ToString());
	}

	TEST_F(TestLuaTypes, TestTableKey) {
		Table::Key kn1(1), kn2(2), kn1a(1);
		Table::Key ks1("1"), ks2("2"), ks1a("1");

		EXPECT_EQ(true, kn1 == kn1a);
		EXPECT_EQ(true, ks1 == ks1a);
		EXPECT_EQ(false, kn1 == ks1);
		EXPECT_EQ(false, ks1 == kn1);
		EXPECT_EQ(true, kn1 < kn2);
		EXPECT_EQ(true, kn1 < ks1);
		EXPECT_EQ(true, ks1 < ks2);

		EXPECT_EQ("1", kn1.ToString());
		EXPECT_EQ("1", ks1.ToString());

	}

	TEST_F(TestLuaTypes, TestLuaTTableIntKey) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		LuaTTable tbl;

		tbl.setValue(Table::Key(1), std::make_shared<LuaTString>("1"));

		// Assure the types are set correctly
		EXPECT_EQ(LUA_TTABLE, tbl.getTypeId());
		EXPECT_EQ("table", tbl.getTypeName(*L));

		// Assure the state is prepared for testing
		EXPECT_NE((LuaState *) NULL, L.get());
		EXPECT_EQ(0, lua_gettop(*L));

		// Assure pushing the value
		EXPECT_NO_THROW(tbl.PushValue(*L));
		EXPECT_EQ(1, lua_gettop(*L));
		EXPECT_EQ(LUA_TTABLE, lua_type(*L, -1));

		// Assure popping the value in relative referencing
		EXPECT_NO_THROW(tbl.PopValue(*L, -1));
		EXPECT_EQ(1,tbl.getValues().size());

		// Assure pooping the value with absolute referencing
		EXPECT_NO_THROW(tbl.PopValue(*L, 1));
		EXPECT_EQ(1, tbl.getValues().size());

		// Assure change of value
		EXPECT_NO_THROW(tbl.setValue(Table::Key(2), std::make_shared<LuaTString>("2")));
		lua_pop(*L,1);
		EXPECT_NO_THROW(tbl.PushValue(*L));
		EXPECT_NO_THROW(tbl.setValue(Table::Key(3), std::make_shared<LuaTString>("2")));
		EXPECT_NO_THROW(tbl.PopValue(*L, 1));
		EXPECT_EQ(2,tbl.getValues().size());

		// Push new value on stacks
		lua_pushnil(*L);

		// Assure popping the value in relative referencing
		EXPECT_THROW(tbl.PopValue(*L, -1), std::invalid_argument);

		// Assure pooping the value with absolute referencing
		EXPECT_THROW(tbl.PopValue(*L, 2), std::invalid_argument);
		
		// Test the value
		EXPECT_EQ("[ \"1\", \"2\" ]",tbl.ToString());

		// Clear the stack
		lua_pop(*L, 2);
		EXPECT_EQ(0, lua_gettop(*L));
		
	}

	TEST_F(TestLuaTypes, TestLuaTTableStrKey) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		LuaTTable tbl;

		tbl.setValue(Table::Key("1"), std::make_shared<LuaTString>("1"));

		// Assure the types are set correctly
		EXPECT_EQ(LUA_TTABLE, tbl.getTypeId());
		EXPECT_EQ("table", tbl.getTypeName(*L));

		// Assure the state is prepared for testing
		EXPECT_NE((LuaState *) NULL, L.get());
		EXPECT_EQ(0, lua_gettop(*L));

		// Assure pushing the value
		EXPECT_NO_THROW(tbl.PushValue(*L));
		EXPECT_EQ(1, lua_gettop(*L));
		EXPECT_EQ(LUA_TTABLE, lua_type(*L, -1));

		// Assure popping the value in relative referencing
		EXPECT_NO_THROW(tbl.PopValue(*L, -1));
		EXPECT_EQ(1,tbl.getValues().size());

		// Assure pooping the value with absolute referencing
		EXPECT_NO_THROW(tbl.PopValue(*L, 1));
		EXPECT_EQ(1, tbl.getValues().size());

		// Assure change of value
		EXPECT_NO_THROW(tbl.setValue(Table::Key("2"), std::make_shared<LuaTString>("2")));
		lua_pop(*L,1);
		EXPECT_NO_THROW(tbl.PushValue(*L));
		EXPECT_NO_THROW(tbl.setValue(Table::Key("3"), std::make_shared<LuaTString>("2")));
		EXPECT_NO_THROW(tbl.PopValue(*L, 1));
		EXPECT_EQ(2,tbl.getValues().size());

		// Push new value on stacks
		lua_pushnil(*L);

		// Assure popping the value in relative referencing
		EXPECT_THROW(tbl.PopValue(*L, -1), std::invalid_argument);

		// Assure pooping the value with absolute referencing
		EXPECT_THROW(tbl.PopValue(*L, 2), std::invalid_argument);
		
		// Test the value
		EXPECT_EQ("{ \"1\" : \"1\", \"2\" : \"2\" }",tbl.ToString());

		// Clear the stack
		lua_pop(*L, 2);
		EXPECT_EQ(0, lua_gettop(*L));
		
	}

	TEST_F(TestLuaTypes, TestLuaTTableMixKey) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		LuaTTable tbl;

		tbl.setValue(Table::Key(1), std::make_shared<LuaTString>("1"));

		// Assure the types are set correctly
		EXPECT_EQ(LUA_TTABLE, tbl.getTypeId());
		EXPECT_EQ("table", tbl.getTypeName(*L));

		// Assure the state is prepared for testing
		EXPECT_NE((LuaState *) NULL, L.get());
		EXPECT_EQ(0, lua_gettop(*L));

		// Assure pushing the value
		EXPECT_NO_THROW(tbl.PushValue(*L));
		EXPECT_EQ(1, lua_gettop(*L));
		EXPECT_EQ(LUA_TTABLE, lua_type(*L, -1));

		// Assure popping the value in relative referencing
		EXPECT_NO_THROW(tbl.PopValue(*L, -1));
		EXPECT_EQ(1,tbl.getValues().size());

		// Assure pooping the value with absolute referencing
		EXPECT_NO_THROW(tbl.PopValue(*L, 1));
		EXPECT_EQ(1, tbl.getValues().size());

		// Assure change of value
		EXPECT_NO_THROW(tbl.setValue(Table::Key("2"), std::make_shared<LuaTString>("2")));
		lua_pop(*L,1);
		EXPECT_NO_THROW(tbl.PushValue(*L));
		EXPECT_NO_THROW(tbl.setValue(Table::Key("3"), std::make_shared<LuaTString>("2")));
		EXPECT_NO_THROW(tbl.PopValue(*L, 1));
		EXPECT_EQ(2,tbl.getValues().size());

		// Push new value on stacks
		lua_pushnil(*L);

		// Assure popping the value in relative referencing
		EXPECT_THROW(tbl.PopValue(*L, -1), std::invalid_argument);

		// Assure pooping the value with absolute referencing
		EXPECT_THROW(tbl.PopValue(*L, 2), std::invalid_argument);
		
		// Test the value
		EXPECT_EQ("{ \"1\" : \"1\", \"2\" : \"2\" }",tbl.ToString());

		// Clear the stack
		lua_pop(*L, 2);
		EXPECT_EQ(0, lua_gettop(*L));
		
	}

	TEST_F(TestLuaTypes, TestLuaTTableTableOpsInt) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		LuaTTable tbl;

		tbl.setValue(Table::Key(1), std::make_shared<LuaTString>("1"));
		tbl.setValue(Table::Key(2), std::make_shared<LuaTString>("2"));
		tbl.setValue(Table::Key(3), std::make_shared<LuaTString>("3"));

		// Assure the types are set correctly
		EXPECT_EQ("1", ((std::shared_ptr<LuaTString> &) tbl.getValues()[Table::Key(1)])->getValue());
		EXPECT_EQ("2", ((std::shared_ptr<LuaTString> &) tbl.getValues()[Table::Key(2)])->getValue());
		EXPECT_EQ("3", ((std::shared_ptr<LuaTString> &) tbl.getValues()[Table::Key(3)])->getValue());
	}

	TEST_F(TestLuaTypes, TestLuaTTableTableOpsStr) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		LuaTTable tbl;

		tbl.setValue(Table::Key("1"), std::make_shared<LuaTString>("1"));
		tbl.setValue(Table::Key("2"), std::make_shared<LuaTString>("2"));
		tbl.setValue(Table::Key("3"), std::make_shared<LuaTString>("3"));

		// Assure the types are set correctly
		EXPECT_EQ("1", ((std::shared_ptr<LuaTString> &) tbl.getValues()[Table::Key("1")])->getValue());
		EXPECT_EQ("2", ((std::shared_ptr<LuaTString> &) tbl.getValues()[Table::Key("2")])->getValue());
		EXPECT_EQ("3", ((std::shared_ptr<LuaTString> &) tbl.getValues()[Table::Key("3")])->getValue());
	}

	TEST_F(TestLuaTypes, TestLuaTTableTableOpsMix) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		LuaTTable tbl;

		tbl.setValue(Table::Key(1), std::make_shared<LuaTString>("1"));
		tbl.setValue(Table::Key("2"), std::make_shared<LuaTString>("2"));
		tbl.setValue(Table::Key(3), std::make_shared<LuaTString>("3"));

		// Assure the types are set correctly
		EXPECT_EQ("1", ((std::shared_ptr<LuaTString> &) tbl.getValues()[Table::Key(1)])->getValue());
		EXPECT_EQ("2", ((std::shared_ptr<LuaTString> &) tbl.getValues()[Table::Key("2")])->getValue());
		EXPECT_EQ("3", ((std::shared_ptr<LuaTString> &) tbl.getValues()[Table::Key(3)])->getValue());

		EXPECT_EQ("1", ((LuaTString &) tbl.getValue(Table::Key(1))).getValue());
		EXPECT_EQ("2", ((LuaTString &) tbl.getValue(Table::Key("2"))).getValue());
	}

	TEST_F(TestLuaTypes, TestLuaTTableIndexOverflow) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		LuaTTable tbl;

		tbl.setValue(Table::Key(1), std::make_shared<LuaTString>("1"));
		tbl.setValue(Table::Key("2"), std::make_shared<LuaTString>("2"));
		tbl.setValue(Table::Key(3), std::make_shared<LuaTString>("3"));

		EXPECT_NO_THROW(tbl.PushValue(*L));
		EXPECT_NO_THROW(tbl.PopValue(*L, 1));
		EXPECT_EQ(3,tbl.getValues().size());

		EXPECT_THROW(tbl.PopValue(*L,-10), std::invalid_argument);
		
	}

	TEST_F(TestLuaTypes, TestLuaTTableAllTypes) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		LuaTTable tbl, tbl2;

		tbl2.setValue(Table::Key("inner-1"), std::make_shared<LuaTString>("1"));
		tbl2.setValue(Table::Key("inner-2"), std::make_shared<LuaTString>("2"));
		tbl2.setValue(Table::Key("inner-3"), std::make_shared<LuaTString>("3"));

		tbl.setValue(Table::Key("nil_will_be_ignored"), std::make_shared<LuaTNil>());
		tbl.setValue(Table::Key("string"), std::make_shared<LuaTString>("2"));
		tbl.setValue(Table::Key("number"), std::make_shared<LuaTNumber>(3.3));
		tbl.setValue(Table::Key("boolean"), std::make_shared<LuaTBoolean>(true));
		tbl.setValue(Table::Key("table"), std::make_shared<LuaTTable>(tbl2));
		

		EXPECT_NO_THROW(tbl.PushValue(*L));
		EXPECT_NO_THROW(tbl.PopValue(*L, 1));
		EXPECT_EQ(1, lua_gettop(*L));
		EXPECT_EQ(4, tbl.getValues().size());
		EXPECT_EQ("{ \"boolean\" : true, \"number\" : 3.300000, \"string\" : \"2\", \"table\" : { \"inner-1\" : \"1\", \"inner-2\" : \"2\", \"inner-3\" : \"3\" } }", tbl.ToString());
		
	}

	TEST_F(TestLuaTypes, TestLuaTypesBridgeToLua) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		LuaTTable tbl, tbl2;

		tbl2.setValue(Table::Key("inner-1"), std::make_shared<LuaTString>("1"));
		tbl2.setValue(Table::Key("inner-2"), std::make_shared<LuaTString>("2"));
		tbl2.setValue(Table::Key("inner-3"), std::make_shared<LuaTString>("3"));

		tbl.setValue(Table::Key("string"), std::make_shared<LuaTString>("1"));
		tbl.setValue(Table::Key("number"), std::make_shared<LuaTNumber>(5.3));
		tbl.setValue(Table::Key("boolean"), std::make_shared<LuaTBoolean>(true));
		tbl.setValue(Table::Key("table"), std::make_shared<LuaTTable>(tbl2));
		

		EXPECT_NO_THROW(tbl.PushValue(*L));
		EXPECT_NO_THROW(tbl.PopValue(*L, 1));
		EXPECT_EQ(1, lua_gettop(*L));
		EXPECT_EQ(4, tbl.getValues().size());

		L.reset();


		testing::internal::CaptureStdout();

		EXPECT_NO_THROW(ctx.CompileFile("test", "TestLuaTypes_1_ok.lua"));
		L = ctx.newStateFor("test");
		EXPECT_NO_THROW(tbl.PushGlobal(*L, "t"));

		EXPECT_EQ(LUA_OK, lua_pcall(*L, 0, LUA_MULTRET, 0));

		EXPECT_NO_THROW(tbl.PopGlobal(*L));

		EXPECT_EQ("function", tbl.getValue(Table::Key("print")).ToString());

		std::string output = testing::internal::GetCapturedStdout();

		EXPECT_EQ("pass\n", output);

		
	}

	TEST_F(TestLuaTypes, TestLuaTypeBaseClass) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();
		
		LuaTString str("test_string");
		LuaTString str2("");

		// Assure the types are set correctly
		EXPECT_EQ(LUA_TSTRING, str.getTypeId());
		EXPECT_EQ("string", str.getTypeName(*L));

		// Push and Pop global
		EXPECT_NO_THROW(str.PushGlobal(*L, "test"));
		EXPECT_NO_THROW(str.PopGlobal(*L));

		EXPECT_EQ("test_string", str.getValue());
		EXPECT_EQ("test", str.getGlobalName());
		EXPECT_EQ(true, str.isGlobal());

		// Pop global in new value
		EXPECT_EQ(false, str2.isGlobal());
		EXPECT_NO_THROW(str2.PopGlobal(*L, "test"));

		EXPECT_EQ("test_string", str2.getValue());
		EXPECT_EQ("test", str2.getGlobalName());
		EXPECT_EQ(true, str2.isGlobal());

		EXPECT_NO_THROW(str.PushValue(*L));
		EXPECT_NO_THROW(str.PopValue(*L));
		EXPECT_EQ(1, lua_gettop(*L));

	}

	TEST_F(TestLuaTypes, TestLuaTUserData) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<LuaState> L = ctx.newState();

		LuaTUserData ud(sizeof(LuaTUserData)), ud2(sizeof(LuaTUserData));

		// Assure the types are set correctly
		EXPECT_EQ(LUA_TUSERDATA, ud.getTypeId());
		EXPECT_EQ("userdata", ud.getTypeName(*L));

		// Assure the state is prepared for testing
		EXPECT_NE((LuaState *) NULL, L.get());
		EXPECT_EQ(0, lua_gettop(*L));

		EXPECT_EQ("userdata", ud.ToString());

		// Assure pushing the value
		EXPECT_NO_THROW(ud.PushValue(*L));
		EXPECT_EQ(1, lua_gettop(*L));
		EXPECT_EQ(LUA_TUSERDATA, lua_type(*L, -1));
		EXPECT_NE((void *) NULL, ud.getRawUserData());

		// Assure popping the value in relative referencing
		EXPECT_NO_THROW(ud.PopValue(*L, -1));
		EXPECT_NE((void *) NULL, ud.getRawUserData());

		// Assure pooping the value with absolute referencing
		EXPECT_NO_THROW(ud.PopValue(*L, 1));
		EXPECT_NE((void *) NULL, ud.getRawUserData());

		// Push new value on stacks
		lua_pushnil(*L);

		// Assure popping the value in relative referencing
		EXPECT_THROW(ud.PopValue(*L, -1), std::invalid_argument);

		// Assure pooping the value with absolute referencing
		EXPECT_THROW(ud.PopValue(*L, 2), std::invalid_argument);
		
		L.reset();
		EXPECT_NO_THROW(ctx.CompileString("test", "print('Calling foo as a metafunction of a usertype ' .. foo(1,2,3,4))"));

		L = ctx.newStateFor("test");

		// Test meta function
		EXPECT_NO_THROW(ud.AddMetaFunction("__call", foo_meta));
		EXPECT_NO_THROW(ud.PushGlobal(*L, "foo"));

		testing::internal::CaptureStdout();
		EXPECT_EQ(0, lua_pcall(*L, 0, LUA_MULTRET, 0));
		std::string output = testing::internal::GetCapturedStdout();

		EXPECT_EQ("Calling foo as a metafunction of a usertype 2.0\n", output);

		// Try to mix the variables

		EXPECT_NO_THROW(ud.PushValue(*L));
		EXPECT_THROW(ud2.PopValue(*L,-1), std::domain_error);
	}


}
