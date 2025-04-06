cmake_minimum_required(VERSION 3.30)
project(Par_alg C)

set(CMAKE_C_STANDARD 23)

find_package(OpenMP REQUIRED)

if(OpenMP_C_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
endif()

find_package(MPI REQUIRED)
if(MPI_C_FOUND)
    include_directories(SYSTEM ${MPI_C_INCLUDE_DIRS})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MPI_C_COMPILE_FLAGS}")
endif()

add_executable(p1 p1.c)
add_executable(p2 p2.c)
add_executable(p3 p3.c)
add_executable(p4 p4.c)

target_link_libraries(p3 MPI::MPI_C)
target_link_libraries(p4 MPI::MPI_C)

