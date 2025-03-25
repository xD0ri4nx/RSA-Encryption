cmake_minimum_required(VERSION 3.10)
project(test_pthreads C)

set(CMAKE_C_STANDARD 23)

# Manually specify GMP paths for MSYS2 MinGW
set(GMP_INCLUDE_DIRS "C:/msys64/mingw64/include")
set(GMP_LIBRARIES "C:/msys64/mingw64/lib/libgmp.a")

# Include GMP headers
include_directories(${GMP_INCLUDE_DIRS})

# Add executable
add_executable(test_pthreads main.c)

# Link GMP library
target_link_libraries(test_pthreads ${GMP_LIBRARIES})