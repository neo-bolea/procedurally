macro(GetCFiles LIB_NAME)
    # Find all source files, public and private headers, and put them in separate variables.
    file(GLOB_RECURSE ${LIB_NAME}_SRCS "*.cpp")
    file(GLOB_RECURSE ${LIB_NAME}_PUB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" 
                                              "${CMAKE_CURRENT_SOURCE_DIR}/include/*.inc")
    file(GLOB_RECURSE ${LIB_NAME}_PRIV_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
                                               "${CMAKE_CURRENT_SOURCE_DIR}/*.inc")
endmacro(GetCFiles)

macro(LibStandard LIB_NAME)
    GetCFiles(${LIB_NAME})

    add_library(${LIB_NAME} ${${LIB_NAME}_SRCS} ${${LIB_NAME}_PRIV_HEADERS} ${${LIB_NAME}_PUB_HEADERS})
    target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

    target_link_libraries(${LIB_NAME} ${ARGN})
    # Configure Visual Studio projects to use multiple cores when building.
    if(MSVC)
        target_compile_options(${LIB_NAME} PRIVATE "/MP")
    endif()
endmacro(LibStandard)