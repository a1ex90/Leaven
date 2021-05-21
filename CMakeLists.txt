cmake_minimum_required(VERSION 3.10)

project(Leaven)

set(CMAKE_CXX_STANDARD 14)

# Enable QT Meta Object Compiler
set(CMAKE_AUTOMOC ON)
# Enable QT User Interface Compiler
set(CMAKE_AUTOUIC ON)
# Enable QT Rource Compiler
set(CMAKE_AUTORCC ON)

if (UNIX)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    endif(OPENMP_FOUND)
endif (UNIX)

# Require QT5
find_package(Qt5 COMPONENTS Core Quick Widgets REQUIRED)

# OpenGLWindow Library
add_subdirectory(ext/QTOpenGLWindow)

# Leaven Library
#add_compile_definitions(USE_DOUBLE)
add_subdirectory(lib)

include_directories(src)
include_directories(lib/src)

# Find all Header and Source files
file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS src/*.h qrc/*.qrc)
file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES src/*.cpp)

# Copy assets to build dir
file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME}
        ${${PROJECT_NAME}_SOURCES}
        ${${PROJECT_NAME}_HEADERS}
        )

target_link_libraries(${PROJECT_NAME}
        QTOpenGLWindow
        LeavenLib
        Qt5::Core
        Qt5::Quick
        )

