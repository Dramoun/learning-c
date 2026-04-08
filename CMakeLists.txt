cmake_minimum_required(VERSION 3.16)

project(Learning C)

set(CMAKE_C_STANDARD 99)

# Required for clangd (LSP)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Put binaries in build/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# =========================
# SDL (manual, vendored)
# =========================

# Your cloned SDL repo lives here:
# project/lib/SDL
add_subdirectory(lib/SDL EXCLUDE_FROM_ALL)

# =========================
# Your source files
# =========================

file(GLOB SRC_FILES src/*.c)

add_executable(main ${SRC_FILES})

# Your headers
target_include_directories(main PRIVATE include)

# =========================
# Link SDL
# =========================

# SDL3 (matches their docs)
target_link_libraries(main PRIVATE SDL3::SDL3)
