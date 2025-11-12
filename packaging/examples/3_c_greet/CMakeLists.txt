cmake_minimum_required(VERSION 3.16)

set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS deployment version")
set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Architectures to produce code for")

project(greet LANGUAGES C)

add_executable(greet main.c
    tcp_connection.h)

if (WIN32)
    target_link_libraries(greet ws2_32)
endif()

# Copy the final executable into the project root (.. from build/)
add_custom_command(TARGET greet POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:greet>
            ${CMAKE_SOURCE_DIR}/$<TARGET_FILE_NAME:greet>
)

# Optional: make "install" target behave normally
include(GNUInstallDirs)
install(TARGETS greet
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
