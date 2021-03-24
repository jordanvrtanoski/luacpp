cmake_minimum_required(VERSION 3.4)
project(luacpp)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
	Lua.hpp
	LuaCpp.cpp LuaCpp.hpp
	Engine/LuaState.cpp Engine/LuaState.hpp
	Engine/LuaType.cpp Engine/LuaType.hpp
	Engine/LuaTString.cpp Engine/LuaTString.hpp
	Engine/LuaTNumber.cpp Engine/LuaTNumber.hpp
	Engine/LuaTBoolean.cpp Engine/LuaTBoolean.hpp
	Engine/LuaTTable.cpp Engine/LuaTTable.hpp
	Registry/LuaRegistry.cpp Registry/LuaRegistry.hpp
	Registry/LuaCodeSnippet.cpp Registry/LuaCodeSnippet.hpp
	Registry/LuaCompiler.cpp Registry/LuaCompiler.hpp
	Registry/LuaCFunction.cpp Registry/LuaCFunction.hpp
	Registry/LuaLibrary.cpp Registry/LuaLibrary.hpp
	LuaContext.cpp LuaContext.hpp
)

include(GNUInstallDirs)


find_package(Lua REQUIRED)

include_directories(example_HelloLua PRIVATE ${LUA_INCLUDE_DIR})

add_library(luacpp SHARED ${SOURCE_FILES})
add_library(luacpp_static STATIC ${SOURCE_FILES})
set_target_properties(luacpp_static PROPERTIES OUTPUT_NAME luacpp)
target_link_libraries(luacpp ${LUA_LIBRARIES})
target_link_libraries(luacpp_static ${LUA_LIBRARIES})

add_executable(hello Example/example_helloworld.cpp)
target_link_libraries(hello luacpp_static)

add_executable(test_lualib Example/example_LuaLibrary.cpp)
target_link_libraries(test_lualib luacpp_static)

add_custom_command(TARGET hello POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/Example/hello.lua ${PROJECT_BINARY_DIR}/hello.lua
	COMMENT "${PROJECT_BINARY_DIR}/hello.lua copied to build" 
)

install(TARGETS luacpp
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(TARGETS luacpp_static
        DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES LuaCpp.hpp DESTINATION "include/LuaCpp")
install(DIRECTORY ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Registry ${CMAKE_SOURCE_DIR}/Engine
	DESTINATION "include/LuaCpp"
        FILES_MATCHING
        PATTERN "*.hpp"
)


# Documents
option(BUILD_DOC "Build documentation" OFF)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${PROJECT_SOURCE_DIR}/../docs/Doxyfile.in)
    set(DOXYGEN_OUT ${PROJECT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
