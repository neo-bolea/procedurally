GetCFiles(Test)
set(Test_LINKS Core
#Common Graphics Math
)

add_executable(UnitTests ${Test_SRCS} ${Test_PRIV_HEADERS} ${Test_PUB_HEADERS})

target_link_libraries(UnitTests ${Test_LINKS})