# TODO:
# Include *.inc in source files OR structure by actual file structure

cmake_minimum_required(VERSION 3.14.4)

include(cmake/extras.cmake)

# Main Project
project(Procedurally)

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

find_package(OpenGL REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}${OPENGL_INCLUDE_DIRS})
link_libraries(${OPENGL_LIBRARIES})


# Other external includes
include_directories("${CMAKE_SOURCE_DIR}/external")

# Platform configuration
if(${PLATFORM} EQUAL x64 OR ${PLATFORM} EQUAL Win32)
    add_definitions(-DWINDOWS)
endif()

# Force include the Common header to all files and add debug information to the lib.
if(MSVC) # VS
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/core/include/Common/Common.h")
    add_definitions(/Z7)
else() # GCC or Clang
    add_definitions(-include "${CMAKE_SOURCE_DIR}/core/include/Common/Common.h")
endif()

set(ALL_PROJECTS Core
#Graphics Common Math
)

# Initialize all projects
add_subdirectory(core)
# add_subdirectory(tests)

# Configure the output directory for the project.
set_target_properties(Core PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${PLATFORM}/${CONFIG}/$<$<CONFIG:Debug>:>")

# Delete previous include folder (in case any headers are newly deleted).
set(PROCEDURALLY_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/build/include")
file(REMOVE "${PROCEDURALLY_INCLUDE_DIR}")

foreach(project ${ALL_PROJECTS})
    # Set default language settings.
    set_target_properties(${project} PROPERTIES CXX_STANDARD 17)
    
    # Copy all public headers to a final include folder.
    file(GLOB_RECURSE PUB_HEADERS_TO_COPY "${CMAKE_SOURCE_DIR}/${project}/include/*.h")
    file(GLOB_RECURSE PUB_INCS_TO_COPY "${CMAKE_SOURCE_DIR}/${project}/include/*.inc")
    set(PUB_HEADERS_TO_COPY ${PUB_HEADERS_TO_COPY} ${PUB_INCS_TO_COPY})

    # For each file to be copied...
    foreach(PUB_HEADER_TO_COPY ${PUB_HEADERS_TO_COPY})
        # ...get the path of the file relative to it's include folder,
        file(RELATIVE_PATH PUB_HEADER_TO_COPY_REL "${CMAKE_SOURCE_DIR}/${project}/include" ${PUB_HEADER_TO_COPY})
        # ...strip the name of the file off of it's path,
        get_filename_component(PUB_HEADER_TO_COPY_REL ${PUB_HEADER_TO_COPY_REL} DIRECTORY)
        # ...so as to copy the file to the final include folder for the project, with it's relative path preserved.
        file(COPY ${PUB_HEADER_TO_COPY} DESTINATION "${PROCEDURALLY_INCLUDE_DIR}/${PUB_HEADER_TO_COPY_REL}")
        # Example: -common/include/Systems/Locator.h => build/include/common/Systems/Locator.h
        # -graphics/include/Camera.h => build/graphics/include/Camera.h
    endforeach()

endforeach()