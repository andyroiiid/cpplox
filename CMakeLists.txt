cmake_minimum_required(VERSION 3.21)
project(cpplox)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpplox
        chunk.cpp chunk.h
        compiler.cpp compiler.h
        compiler_context.cpp compiler_context.h
        forward.h
        non_copyable.h
        object.cpp object.h
        op_code.h
        scanner.cpp scanner.h
        singleton.h
        table.cpp table.h
        token.h token.cpp
        value.cpp value.h
        vm.cpp vm.h
        main.cpp)

target_compile_definitions(cpplox PRIVATE DEBUG_PRINT_CODE)