cmake_minimum_required (VERSION 2.6)
project (Lovecraft)
# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)

# add includes
include_directories(include)

# add sources
file(GLOB SOURCES "src/*.c")


# add lib
# add_library(testStudent SHARED ${SOURCES})


# add exe
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY, "bin/")
add_executable (lovecraft src/main.c)

# add install path
install(TARGETS lovecraft RUNTIME DESTINATION /usr/local/bin)