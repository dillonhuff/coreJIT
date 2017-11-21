cmake_minimum_required(VERSION 2.6)

project(coreJIT)

INCLUDE_DIRECTORIES(./src/)

SET(EXTRA_CXX_COMPILE_FLAGS "-std=c++11 -I./src -I./test -I/opt/local/include -I/Users/dillon/CppWorkspace/coreir/include/ -O2 -Werror -Wall")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_COMPILE_FLAGS}")

SET(CXX_OCL_LINK_FLAGS "-lcoreir -lcoreir-commonlib -L/Users/dillon/CppWorkspace/coreir/lib/")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${CXX_OCL_LINK_FLAGS}")

INCLUDE_DIRECTORIES(./src/)

SET(SRC_FILES ./src/corejit.cpp
              ./src/mixed_interpreter.cpp)

SET(TEST_FILES ./test/main.cpp
               ./test/mixed_jit_tests.cpp)

add_executable(all-tests ${TEST_FILES} ${SRC_FILES})
