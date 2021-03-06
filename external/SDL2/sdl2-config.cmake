set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")

set(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/${PLATFORM}/${CONFIG}/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/${PLATFORM}/${CONFIG}/SDL2main.lib")
set(SDL2_LIBRARY_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/${PLATFORM}/${CONFIG}")

string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)