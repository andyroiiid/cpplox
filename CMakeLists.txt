cmake_minimum_required(VERSION 3.21)
project(cpplox)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpplox
        non_copyable.h
        singleton.h
        value.cpp value.h
        object.cpp object.h
        chunk.cpp chunk.h
        scanner.cpp scanner.h
        compiler.cpp compiler.h
        vm.cpp vm.h
        main.cpp)

target_compile_definitions(cpplox PRIVATE DEBUG_PRINT_CODE DEBUG_TRACE_EXECUTION)