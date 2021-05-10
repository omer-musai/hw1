cmake_minimum_required(VERSION 3.0.0)
project(HW1PROJECT VERSION 0.1.0 LANGUAGES C)


set(CMAKE_C_STANDARD 99)

set(MTM_FLAGS_DEBUG " -std=gnu99 -Wall -Werror -pedantic-errors -g")
set(MTM_FLAGS_RELEASE "${MTM_FLAGS_DEBUG} -DNDEBUG -std=c99")

set(HEADER_FILES ${workspaceFolder}/mtm_map/map.h)

set(CMAKE_C_FALGS ${MTM_FLAGS_DEBUG})

add_executable(map map.c)
