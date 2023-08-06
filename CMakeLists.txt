cmake_minimum_required(VERSION 3.15)
project(disc_filler)

set(CMAKE_CXX_STANDARD 20)

add_executable(disc_filler
    main.cpp
    arguments.cpp
    directory.cpp
)

target_link_libraries(disc_filler PRIVATE)
