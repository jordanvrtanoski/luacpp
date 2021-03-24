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

#include "LuaCodeSnippet.hpp"

using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;

LuaCodeSnippet::LuaCodeSnippet() : code() {
	code.clear();
}

int LuaCodeSnippet::WriteCode(unsigned char* buff, size_t size) {
	unsigned char *end = (unsigned char *)buff+size;
	try {
		code.insert(code.end(),buff,end);
		return 0;
	} catch (...) {
		return 1;
	}
}	

int LuaCodeSnippet::getSize() {
	return code.size();
}

void LuaCodeSnippet::setName(std::string _name) {
	name = std::move(_name);
}

std::string LuaCodeSnippet::getName() {
	return name;
}

const char *LuaCodeSnippet::getBuffer() {
	return (const char *)&code[0];
}

void LuaCodeSnippet::UploadCode(LuaState &L) {
	lua_load(L, code_reader, this, (const char *)name.c_str(), NULL);
}

int code_writer (lua_State* L, const void* p, size_t size, void* u) {
	return ((LuaCodeSnippet *) u)->WriteCode((unsigned char*)p, size);
}

const char * code_reader (lua_State *L, void *data, size_t *size) {
	*size = ((LuaCodeSnippet *)data)->getSize();
	return ((LuaCodeSnippet *)data)->getBuffer();
}
