cmake_minimum_required(VERSION 3.30)
project(gas_simulation C)

set(CMAKE_C_STANDARD 11)

add_executable(gas_simulation main.c)
add_executable(gas_osc gas_osc.c)
add_executable(gas_cont gas_cont.c)