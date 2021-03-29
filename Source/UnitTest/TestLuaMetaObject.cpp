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
#include <map>

#include "../LuaCpp.hpp"
#include "gtest/gtest.h"

using namespace LuaCpp::Engine;

namespace LuaCpp {

	class MetaMap : public LuaMetaObject {
		public:
		        std::map<std::string, std::shared_ptr<LuaType>> values;	
			MetaMap() : values() {}
			bool inline Exists(const std::string &name) {
				return !(values.find( name ) == values.end());
			}

			std::shared_ptr<LuaType> getValue(int key) {
				if (Exists(std::to_string(key))) {
					return values[std::to_string(key)];
				}
				return std::make_shared<LuaTNil>();
			}

			std::shared_ptr<LuaType> getValue(std::string &key) {
				if (Exists(key)) {
					return values[key];
				}
				return std::make_shared<LuaTNil>();
			}

			void setValue(int key, std::shared_ptr<LuaType> val) {
				values[std::to_string(key)] = val;
			}

			void setValue(std::string &key, std::shared_ptr<LuaType> val) {
				values[key] = val;
			}

	};

	class TestLuaMetaObject : public ::testing::Test {
	  protected:
		virtual void SetUp() {
			std::ofstream of("TestLuaMetaObject_1_ok.lua", std::ofstream::out | std::ofstream::trunc );
			of << "print(\"Hello World from Lua\")";
			of.close();
		}
	};

	TEST_F(TestLuaMetaObject, TestMetaObject) {
		LuaContext ctx;

		std::unique_ptr<Engine::LuaState> L = ctx.newState();
		
		std::shared_ptr<LuaMetaObject> obj = std::make_shared<LuaMetaObject>();
		std::string key = "0";

		EXPECT_EQ(LUA_TNIL, obj->getValue(0)->getTypeId());
		EXPECT_EQ(LUA_TNIL, obj->getValue(key)->getTypeId());

		EXPECT_NO_THROW(obj->setValue(0, obj));
		EXPECT_NO_THROW(obj->setValue(key, obj));

		
		LuaTString str("1");
		LuaTTable tbl();
		LuaTNumber nbr(1);
		LuaTBoolean bol(true);
		LuaTNil nil();
		
	}

	TEST_F(TestLuaMetaObject, TestCPPMapFromLua) {
		/**
		 * Basic test getting instance of the `lua_State *`
		 */
		LuaContext ctx;

		std::unique_ptr<Engine::LuaState> L = ctx.newState();

		std::shared_ptr<MetaMap> mm = std::make_shared<MetaMap>();
		std::shared_ptr<MetaMap> aa = std::make_shared<MetaMap>();

		EXPECT_NE((Engine::LuaState *) NULL, L.get());
		EXPECT_EQ(0, lua_gettop(*L));
		EXPECT_NE((const char *) NULL, lua_pushstring(*L, "test"));
		EXPECT_EQ(1, lua_gettop(*L));

		EXPECT_NO_THROW(ctx.AddGlobalVariable("mm", mm));
		EXPECT_NO_THROW(ctx.AddGlobalVariable("aa", mm));

		testing::internal::CaptureStdout();
		EXPECT_NO_THROW(ctx.CompileStringAndRun("print(mm[\"1\"])"));
		EXPECT_NO_THROW(ctx.CompileStringAndRun("print(mm[1])"));
		std::string output = testing::internal::GetCapturedStdout();
		EXPECT_EQ("nil\nnil\n", output);

		testing::internal::CaptureStdout();
		EXPECT_NO_THROW(ctx.CompileStringAndRun("mm[\"1\"] = \"test\""));
		EXPECT_NO_THROW(ctx.CompileStringAndRun("print(mm[\"1\"])"));
		EXPECT_NO_THROW(ctx.CompileStringAndRun("print(mm[1])"));
		output = testing::internal::GetCapturedStdout();
		EXPECT_EQ("test\ntest\n", output);

		testing::internal::CaptureStdout();
		EXPECT_NO_THROW(ctx.CompileStringAndRun("mm[2] = \"test\""));
		EXPECT_NO_THROW(ctx.CompileStringAndRun("print(mm[\"2\"])"));
		EXPECT_NO_THROW(ctx.CompileStringAndRun("print(mm[2])"));
		output = testing::internal::GetCapturedStdout();
		EXPECT_EQ("test\ntest\n", output);

		testing::internal::CaptureStdout();
		EXPECT_NO_THROW(ctx.CompileStringAndRun("aa[1] = \"test\""));
		EXPECT_NO_THROW(ctx.CompileStringAndRun("print(aa[1])"));
		output = testing::internal::GetCapturedStdout();
		EXPECT_EQ("test\n", output);

		testing::internal::CaptureStdout();
		EXPECT_NO_THROW(ctx.CompileStringAndRun("aa[2] = true"));
		EXPECT_NO_THROW(ctx.CompileStringAndRun("print(tostring(aa[2]))"));
		output = testing::internal::GetCapturedStdout();
		EXPECT_EQ("true\n", output);
		
		testing::internal::CaptureStdout();
		EXPECT_NO_THROW(ctx.CompileStringAndRun("aa[3] = 1.1"));
		EXPECT_NO_THROW(ctx.CompileStringAndRun("print(aa[3])"));
		output = testing::internal::GetCapturedStdout();
		EXPECT_EQ("1.1\n", output);

		testing::internal::CaptureStdout();
		EXPECT_NO_THROW(ctx.CompileStringAndRun("aa[4] = {\"1\"}"));
		EXPECT_NO_THROW(ctx.CompileStringAndRun("print(aa[4][1])"));
		output = testing::internal::GetCapturedStdout();
		EXPECT_EQ("1\n", output);

		testing::internal::CaptureStdout();
		EXPECT_NO_THROW(ctx.CompileStringAndRun("aa[5] = print"));
		EXPECT_NO_THROW(ctx.CompileStringAndRun("print(aa[5])"));
		output = testing::internal::GetCapturedStdout();
		EXPECT_EQ("function\n", output);

		EXPECT_NO_THROW(ctx.CompileStringAndRun("aa()"));
	}
}


