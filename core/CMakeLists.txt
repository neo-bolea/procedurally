GetCFiles(Core)
set(Core_LINKS Common Graphics Math)

add_executable(Core ${Core_SRCS} ${Core_PRIV_HEADERS} ${Core_PUB_HEADERS})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Core)

#Copy SDL
add_custom_command(
    TARGET Core
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${SDL2_INCLUDE_DIRS}/../lib/${PLATFORM}/${CONFIG}/SDL2.dll"
	"$<TARGET_FILE_DIR:Core>/SDL2.dll"
)

#Copy GLEW
add_custom_command(
    TARGET Core
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}${GLEW_INCLUDE_DIRS}/../lib/${PLATFORM}/${CONFIG}/glew32.lib"
	"$<TARGET_FILE_DIR:Core>/glew32.dll"
)

target_link_libraries(Core ${Core_LINKS})
if(MSVC)
    target_compile_options(Core PRIVATE "/MP")
endif()