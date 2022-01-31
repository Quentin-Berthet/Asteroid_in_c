cmake_minimum_required(VERSION 3.16)
project(tp_asteroids C)

set(CMAKE_C_STANDARD 11)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(GCC_COMPILE_FLAGS "-Wall -Wextra -Wconversion -pedantic -fsanitize=undefined,null,bounds,thread")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    set(GCC_COMPILE_FLAGS "-O3")
endif ()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    execute_process(COMMAND sdl2-config --cflags OUTPUT_VARIABLE SDL2_C_FLAGS)
    include_directories(${SDL2_C_FLAGS})
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    include_directories(/usr/local/include)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COMPILE_FLAGS}")

add_executable(tp_asteroids graphics/main.c
        asteroids/asteroids.c
        asteroids/asteroids.h
        c_vector/vector.c
        c_vector/vector.h
        geom/dyn_params.c
        geom/dyn_params.h
        geom/dynamics.c
        geom/dynamics.h
        geom/triangle.c
        geom/triangle.h
        geom/utils.c
        geom/utils.h
        geom/vec.c
        geom/vec.h
        graphics/actions.c
        graphics/actions.h
        graphics/gfx.c
        graphics/gfx.h
        vessel/bullet.c
        vessel/bullet.h
        vessel/vessel.c
        vessel/vessel.h
        threads/ast_params.c
        threads/ast_params.h
        threads/bullets_params.c
        threads/bullets_params.h)

target_link_libraries(tp_asteroids SDL2 m pthread)
