GetCFiles(Test)

add_executable(UnitTests ${Test_SRCS} ${Test_PRIV_HEADERS} ${Test_PUB_HEADERS})
target_include_directories(UnitTests PUBLIC "${CMAKE_SOURCE_DIR}/external" "${CMAKE_SOURCE_DIR}/build/include")
target_link_libraries(UnitTests ProcedurallyCore)