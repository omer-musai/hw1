cmake_minimum_required(VERSION 3.0.0)
project(HW1PROJECT VERSION 0.1.0 LANGUAGES C)


set(CMAKE_C_STANDARD 99)

set(MTM_FLAGS_DEBUG " -std=gnu99 -Wall -Werror -pedantic-errors -g")
set(MTM_FLAGS_RELEASE "${MTM_FLAGS_DEBUG} -DNDEBUG -std=c99")

set(HEADER_FILES ${workspaceFolder}/mtm_chess/chessSystem.h)

set(CMAKE_C_FALGS ${MTM_FLAGS_DEBUG})

link_directories(.)

add_executable(chess chessSystem.c)

target_link_libraries(chess libmap.a)