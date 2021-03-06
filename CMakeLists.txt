
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

cmake_minimum_required(VERSION 3.1)

# Not even one version out yet :(
project(Red_Crane_Engine VERSION 0.0 LANGUAGES C CXX)

# Tell C++ about our install path
add_definitions(-DREDC_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")
add_definitions(-DREDC_SHARE_DIR="${PROJECT_NAME}")

# Right now we're using OpenGL.
add_definitions(-DREDC_USE_OPENGL)

# Use radians everywhere
add_definitions(-DGLM_FORCE_RADIANS)

# Include our own modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules/ ${CMAKE_MODULE_PATH})

# We want those shiny new features!
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_FIND_FRAMEWORK LAST)

# Build freetype-gl
add_subdirectory(freetype-gl)

include(data_target)

find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)

add_subdirectory(soloud)
add_subdirectory(gl)
add_subdirectory(opensimplex)
add_subdirectory(mods)
add_subdirectory(assets)
add_subdirectory(survival)
add_subdirectory(mildly-cold)
add_subdirectory(broomgame)
add_subdirectory(src)
add_subdirectory(tests)

# Copy the run script to the build directory but don't install!
add_custom_target(runscript ALL)
add_custom_command(TARGET runscript COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/run.sh
        ${CMAKE_CURRENT_BINARY_DIR}/run.sh
        MAIN_DEPENDENCY run.sh)
