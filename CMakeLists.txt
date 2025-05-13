cmake_minimum_required(VERSION 3.10.0)
project(mtar VERSION 0.1.0 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src)
add_executable(mtar main.c src/mtar.c)

