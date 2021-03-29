cmake_minimum_required(VERSION 3.16)
project(stannox C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-O3")

add_executable(main src/app.c src/camera.c src/math.c src/shader.c src/voxel.c include/app.h include/camera.h include/shader.h include/voxel.h include/snx/math/constants.h include/snx/math/math.h include/snx/math/types.h)
target_include_directories(main PRIVATE include)
target_link_libraries(main PRIVATE glad glfw m)

add_library (glad STATIC src/glad.c)
target_include_directories (glad PRIVATE include)
if (DEFINED CMAKE_DL_LIBS)
    target_link_libraries (glad PRIVATE ${CMAKE_DL_LIBS})
endif ()
