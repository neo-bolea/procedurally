# TODO:
# Include *.inc in source files OR structure by actual file structure

cmake_minimum_required(VERSION 3.14.4)

include(cmake/extras.cmake)

# Main Project
project(Procedurally)

# Defines
set(CMAKE_INSTALL_PREFIX "C:/")
set(MSVC on)

if(MSVC)
    set(CONFIG $(Configuration))
    set(PLATFORM $(Platform))
else()
    set(CONFIG Release)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    	set(PLATFORM x64)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    	set(PLATFORM Win32)
    endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# SDL
set(SDL2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
link_libraries(${SDL2_LIBRARIES})

# OpenGL/glew
set(GLEW_LIBRARIES "/external/glew/lib/${PLATFORM}/${CONFIG}/glew32s.lib;")
set(GLEW_INCLUDE_DIR "/external/glew/include")
set(GLEW_USE_STATIC_LIBS ON)
find_library(GLEW REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}${GLEW_INCLUDE_DIR})
link_libraries(${CMAKE_SOURCE_DIR}${GLEW_LIBRARIES})
message(DIR: ${CMAKE_SOURCE_DIR}${GLEW_INCLUDE_DIR})
message(LIB: ${CMAKE_SOURCE_DIR}${GLEW_LIBRARIES})

find_package(OpenGL REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}${OPENGL_INCLUDE_DIRS})
link_libraries(${OPENGL_LIBRARIES})

# Configuration
list(APPEND CMAKE_CXX_FLAGS "/std:c++17")
add_definitions(-DWINDOWS)

if(MSVC) # VS
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/common/include/Common.h")
else() # GCC or Clang
    add_definitions(-include "${CMAKE_SOURCE_DIR}/common/include/Common.h")
endif()

add_subdirectory(common)
add_subdirectory(math)
add_subdirectory(graphics)
add_subdirectory(core)
add_subdirectory(tests)

set(ALL_PROJECTS Common Graphics Core Math)

foreach(project ${ALL_PROJECTS})
    set_target_properties(${project} PROPERTIES CXX_STANDARD 17)
endforeach()