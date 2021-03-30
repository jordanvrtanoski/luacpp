# Introduction

LuaCpp is a C++ library that wraps the `liblua` library and it's `C` API to provides a set of high level C++ classes that are managing the complexity of the Lua integaration.

Adding lua support can be as simple as adding few lines of code to the project. Following is the LuaCpp Hello World example

```c++
#include <LuaCpp.hpp>
#include <iostream>

using namespace LuaCpp;
using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;

int main(int argc, char **argv) {

	std::cout << "Hi from C++, this is a demo how LuaCpp can be used" << "\n";
	LuaContext lua;

	// The simples way is to use CompileStringAndRun method
	try {
		lua.CompileStringAndRun("print('The fastest way to start using lua in a project')");
	}
	catch (std::runtime_error& e)
  	{
		std::cout << e.what() << '\n';
  	}
}
```

To have the information about the `C++` classes and utility functions, make the `doc_pdf` target which
generates the full reference and user manual.

## Executing Lua Code in C++ project

The `lua` engine is wrapped in a LuaContext object that provides the highest abstraction. 
There is no need to understand the lua engine in order to add the support to the C++ project.
The communication with the engine is trough the `LuaContext` instance.

The `LuaContext` allows execution of `lua` code as a string, or in a file.
The lua code will be compiled and stored in the internal registry for reuse, avoiding
the time consuming compilation of the large code blocks.
The exception from this behaviour are the `CompileStringAndRun()` and `CompileFileAndRun()` which 
will always recompile the provided code under the name `default`.

The developer can add global variables in the context. The variables should be `LuaType` (ex. `LuaTString`, `LuaTNumber`, etc.).
The global variables are set in the lua engine (`LuaState`) before the code snippet is executed, so they will be
available to the script code running in the engine. 
The lua code can modify the global variables. After the execution of the code, the global variables are
copied back ot the LuaContext and the modifications are available. Drawback of this approach is that there is
copying of the variables before and after exectuion which is addtional work (time) required from the engine. LuaCpp resolves this
by allowing the `MetaObjects` which are described in later chapter.

> Warning:
> Lua is not thread safe library, so the LuaContext instances are also not thread safe.
> If the use is required in multithreaded environment, best approach is to create separate
> context instnce for each of the threads. 

Following is the example of use of the global variables

```c++
#include <LuaCpp.hpp>
#include <iostream>

using namespace LuaCpp;
using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;

int main(int argc, char **argv) {
	LuaContext ctx;

	std::shared_ptr<Engine::LuaTString> str = std::make_shared<Engine::LuaTString>("world from C++!");

	ctx.AddGlobalVariable("world", str));
	ctx.CompileString("test", "print('Hello '..world) world = 'world from lua!'");
	try {
		ctx.Run("test");
	}
	catch (std::runtime_error& e)
  	{
		std::cout << e.what() << '\n';
  	}

	std::cout << "Hello " << str->getValue() << "\n";
}
```


## Instrumenting existing C++ objects

Library also provides a MetaObject that can be used to instrument the existing C++ objects. 
There is no need to change the structure or the behaviour of the existing objects. 
The only requirement is for the object to inherit the LuaMetaObject and to implement 3 methods. 
The instrumented C++ objects are available in the lua engine (as a lua types) withouth the need 
to move the whole object data in the lua engine as standard lua types. 
The objects will behave as a native lua types. 

Here is example of instrumenting the `std::map`:

```c++
class MetaMap : public LuaMetaObject {
	public:
	        std::map<std::string, std::shared_ptr<LuaType>> values;	
		MetaMap() : values() {}
		bool inline Exists(const std::string &name) {
			return !(values.find( name ) == values.end());
		}

		std::shared_ptr<LuaType> getValue(std::string &key) {
			if (Exists(key)) {
				return values[key];
			}
			return std::make_shared<LuaTNil>();
		}

		void setValue(std::string &key, std::shared_ptr<LuaType> val) {
			values[key] = val;
		}

};
```

## Adding Custom Libraries to lua engine

LuaCpp allows you to register custom `C` functions as a library in the LuaContext. The libarary will
be uploaded to the LuaState whenever a new state is instantiated (similar to the global variables). 
Creating library will require understanding of the low-level lua and LuaCpp functions.

The library can contin multiple functions that should be registred under different names. The names can not 
overlap, however, the function call will receive all arguments that are passed from the lua invocation, so the
function can implement different behaviours based on the number and type of the arguments. 

Following is an example of a `C` library function which will sum the numerical arguments provided to the function:

```c++
#include <LuaCpp.hpp>
#include <iostream>
#include <stdexcept>

using namespace LuaCpp;
using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;

extern "C" {
   static int _sum (lua_State *L) {
     int n = lua_gettop(L);    /* number of arguments */
     lua_Number sum = 0.0;
     int i;
     for (i = 1; i <= n; i++) {
       if (!lua_isnumber(L, i)) {
         lua_pushliteral(L, "incorrect argument");
         lua_error(L);
       }
       sum += lua_tonumber(L, i);
     }
     lua_pushnumber(L, sum);         /* second result */
     return 1;                   /* number of results */
   }
}

int main(int argc, char **argv) {

	// Creage Lua context
	LuaContext lua;

	// Create library "foo" conatining the "foo" function
	std::shared_ptr<LuaLibrary> lib = std::make_shared<LuaLibrary>("foolib");
	lib->AddCFunction("sum", _sum);

	// Add library to the context
	lua.AddLibrary(lib);

	// Run the context	
	try {
		lua.CompileStringAndRun("print(\"Result of calling foolib.sum(1,2,3,4) = \" .. foolib.sum(1,2,3,4))");
	}
	catch (std::runtime_error& e)
  	{
		std::cout << e.what() << '\n';
  	}
}
```


## Installing

Clone the project and from the root of the project, invoke:

```
mkdir build
cd build
cmake ../Source
make -j 4
make install
```


## Building documents

```
sudo apt-get install texlive-latex-base texlive-latex-recommended texlive-latex-extra graphviz

mkdir build
cd build
cmake ../Source
make doc_pdf

```

## Testing the memory management

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../Source

make -j 4
make test_memory
```

## Running the unit test and debugging

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../Source

make -j 4
make test 
```

## Running the coverage test

```
apt install gcovr
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Coverage ../Source

make -j 4
make coverage-cli
make coverage-html
```

