# TODO:
# Include *.inc in source files OR structure by actual file structure

cmake_minimum_required(VERSION 3.14.4)

include(cmake/extras.cmake)

# Defines
set(MSVC on)
set(CONFIG Release)

set(PLATFORM x64)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(PLATFORM x64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(PLATFORM Win32)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# SDL
set(SDL2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
link_libraries(${SDL2_LIBRARIES})

# OpenGL/glew
set(GLEW_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/external/glew/include")
set(GLEW_LIBRARY "${PROJECT_SOURCE_DIR}/external/glew/lib/x64/Release/glew32s.lib")
set(GLEW_USE_STATIC_LIBS ON)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}${OPENGL_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}${GLEW_INCLUDE_DIRS})
link_libraries(${OPENGL_LIBRARIES})
link_libraries(${CMAKE_SOURCE_DIR}${GLEW_LIBRARIES})


# Configuration
list(APPEND CMAKE_CXX_FLAGS "/std:c++17")
add_definitions(-DWINDOWS)

if(MSVC) # VS
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/common/include/Common.h")
else() # GCC or Clang
    add_definitions(-include "${CMAKE_SOURCE_DIR}/common/include/Common.h")
endif()

# Main Project
project(Procedurally)

add_subdirectory(common)
add_subdirectory(math)
add_subdirectory(graphics)
add_subdirectory(core)
add_subdirectory(tests)

set(ALL_PROJECTS Common Graphics Core Math)

foreach(project ${ALL_PROJECTS})
    set_target_properties(${project} PROPERTIES CXX_STANDARD 17)
endforeach()