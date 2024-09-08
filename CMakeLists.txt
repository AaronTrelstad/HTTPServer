cmake_minimum_required(VERSION 3.28)
project(HTTPServer C)

set(CMAKE_C_STANDARD 11)

add_executable(HTTPServer
        handler.c
        server.h
        server.c
        utils.c)

set(STATIC_FILES_DIR ${CMAKE_SOURCE_DIR}/UI)

set(BUILD_STATIC_FILES_DIR ${CMAKE_BINARY_DIR}/UI)

add_custom_command(
        TARGET HTTPServer
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${STATIC_FILES_DIR}
        ${BUILD_STATIC_FILES_DIR}
)

