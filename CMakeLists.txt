cmake_minimum_required(VERSION 3.14.4)

include(cmake/extras.cmake)

# Main Project
project(Procedurally)

# Stop ZERO_CHECK project from being generated.
set(CMAKE_SUPPRESS_REGENERATION true)

# Defines
set(CMAKE_INSTALL_PREFIX "C:/")
set(MSVC on)

# Set default values for CONFIG and PLATFORM
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

### SDL ###
set(SDL2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
link_libraries(${SDL2_LIBRARIES})

### OpenGL/glew ###
set(GLEW_LIBRARIES "/external/glew/lib/${PLATFORM}/${CONFIG}/glew32s.lib;")
set(GLEW_INCLUDE_DIR "/external/glew/include")
set(GLEW_USE_STATIC_LIBS ON)
find_library(GLEW REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}${GLEW_INCLUDE_DIR})
link_libraries(${CMAKE_SOURCE_DIR}${GLEW_LIBRARIES})

find_package(OpenGL REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}${OPENGL_INCLUDE_DIRS})
link_libraries(${OPENGL_LIBRARIES})

### Boost ###
set(BOOST_INCLUDE_DIR "/external/Boost")
set(BOOST_LIBRARY_DIR "/external/Boost/stage/lib")
file(GLOB BOOST_LIBRARIES "${CMAKE_SOURCE_DIR}${BOOST_LIBRARY_DIR}/*.lib")

include_directories(${CMAKE_SOURCE_DIR}${BOOST_INCLUDE_DIR})


# Other external includes
include_directories("${CMAKE_SOURCE_DIR}/external")

# Platform configuration
if(${PLATFORM} EQUAL x64 OR ${PLATFORM} EQUAL Win32)
    add_definitions(-DWINDOWS)
endif()

# Force include the Common header to all files and add debug information to the lib.
if(MSVC) # VS
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/include/Common/Common.h")
    add_definitions(/Z7)
else() # GCC or Clang
    add_definitions(-include "${CMAKE_SOURCE_DIR}/src/include/Common/Common.h")
endif()

# Initialize all projects
set(PROJECTS ProcedurallyCore UnitTests)

add_subdirectory(src)
add_subdirectory(unit_tests)


### Boost ###
# Separate all Boost libraries between Debug and Release libraries.
GetBoostLibraries()

# Link different Boost libraries depending on whether the Game is in Debug or Release mode.
# The function has to be called again for every single library, so a loop is used.
foreach(Library ${BOOST_LIBRARIES_RELEASE})
    target_link_libraries(ProcedurallyCore optimized ${Library})
endforeach()

foreach(Library ${BOOST_LIBRARIES_DEBUG})
    target_link_libraries(ProcedurallyCore debug ${Library})
endforeach()


# Set the default startup project for VS projects
if(MSVC)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ProcedurallyCore)
endif()

# Configure the output directory for the project.
set_target_properties(ProcedurallyCore PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${PLATFORM}/${CONFIG}/$<$<CONFIG:Debug>:>")

# Set the C++ version.
set_property(TARGET ${PROJECTS} PROPERTY CXX_STANDARD 17)

add_dependencies(UnitTests ProcedurallyCore)