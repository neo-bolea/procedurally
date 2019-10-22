set(IO_SRCS IOPath.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(IO ${IO_SRCS})
#cotire(IO)