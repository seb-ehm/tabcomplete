project(console)

add_executable(console_test main.cpp)

add_library(consolero STATIC consolero.h consolero.cpp)

target_link_libraries(console_test PRIVATE consolero)