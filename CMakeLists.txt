cmake_minimum_required(VERSION 3.1)
project(semaphore_city)
include_directories(.)
set(CMAKE_GCC_FLAGS "-Wall -lpthread -lrt -lm `allegro-config --libs`")

set(THREADS_PREFER_PTHREAD_FLAG ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
find_package(Allegro REQUIRED)
find_package(Threads REQUIRED)
find_package(ptask REQUIRED)

#set(ptask_INCLUDE_DIRS "${ptask_DIR}/include")
set(ptask_INCLUDE_DIRS "${ptask_DIR}/include")
message("Include dir ${ptask_INCLUDE_DIRS}")

add_executable(application src/main.c src/visualization.c src/cars.c)

include_directories ("${ptask_INCLUDE_DIRS}")
include_directories(${ALLEGRO_INCLUDE_DIR})

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries


target_link_libraries(application ptask Threads::Threads ${ALLEGRO_LIBRARY} rt m)
