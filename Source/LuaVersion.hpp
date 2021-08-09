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

#ifndef LUACPP_LUAVERSION_HPP
#define LUACPP_LUAVERSION_HPP

#define LUACPP_VERSION_MAJOR		0
#define LUACPP_VERSION_MINOR		2	
#define LUACPP_VERSION_REV		0

/**
 *@brief The main namespace under which the LuaCpp and it's components will be located
 */
namespace LuaCpp {
	
	/**
	 *@brief Version of the library
	 */
	extern const char Version[];
}

#endif //LUACPP_LUAVERSION_HPP
