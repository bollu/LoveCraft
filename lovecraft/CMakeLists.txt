cmake_minimum_required (VERSION 2.6)
project (Lovecraft)


# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)


# Includes
# ========
include_directories("include/")

# Sources
# =======
file(GLOB SOURCES "src/*.c")


# Executable
# ==========
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY, "bin/")
add_executable(lovecraft ${SOURCES})

# Install
# ============
install(TARGETS lovecraft RUNTIME DESTINATION /usr/local/bin)

# Compiler flags
# ==============
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror" )
    
# Libraries
# =========
# target_link_libraries(lovecraft PUBLIC libgl.a)
target_link_libraries(lovecraft PUBLIC libSDL2.a)
target_link_libraries(lovecraft PUBLIC libGLEW.a)

if(APPLE)
    INCLUDE_DIRECTORIES(/System/Library/Frameworks)
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
    SET(EXTRA_LIBS ${OpenGL_LIBRARY})
endif(APPLE)

target_link_libraries(lovecraft PUBLIC ${EXTRA_LIBS})

# target_link_libraries(lovecraft PUBLIC OpenGL)
# -framework OpenGL
# add_library(testStudent SHARED ${SOURCES})



