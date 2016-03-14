cmake_minimum_required (VERSION 2.6)
project (Lovecraft)

# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)

# Executable
# ==========
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY, "bin/")
add_executable (lovecraft src/main.c)

# Includes
# ========
include_directories(include)

# Sources
# =======
file(GLOB SOURCES "src/*.c")


# Libraries
# =========
target_link_libraries(lovecraft PUBLIC libsdl2.a)

# add_library(testStudent SHARED ${SOURCES})



# Install
# ============
install(TARGETS lovecraft RUNTIME DESTINATION /usr/local/bin)