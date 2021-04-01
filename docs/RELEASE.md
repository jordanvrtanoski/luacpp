# Release numbering and identification

The release numbering follows the [semantic versioning](https://semver.org/).

Brieffly, the version is made out of 3 numbers: Major, Minor and Revision.

- Major number will be changed when the release renders some (or all) of the APIs incompatible, so the migration from lower releases to this relase will require code refactoring
- Minor number will be updated when the functionality is added in a backward compatible manner
- Revision number will be updated when a patches and defect fixes are released.

The current version of the libary has the `0` as a Major release, which means the project is still in early development phase and not ready to lock the API footprint. This means that anythign can change at any time.

##

## Location of the release identifiers

The release number(s) can be obtained from two locations in the code, and one location in the documents.
The code release marks are in:

**Source/LuaCpp.hpp**
```
#define LUACPP_VERSION_MAJOR		0
#define LUACPP_VERSION_MINOR		1	
#define LUACPP_VERSION_REV		0
```
The macros can be used by the build system to identify the release version in order to allow compiling the project which uses LuaCpp in more than one version.

**Source/LuaCpp.cpp**
```
const char LuaCpp::Version[] = "0.1.0";
```

This variable can be used in runtime in order to decide the version of the LuaCpp. The number will also be available in the `lua` context as `_luacppversion`.

The documentation has the identifire in:

**docs/Doxyfile.in**
```
PROJECT_NUMBER         = 0.1.0
```
