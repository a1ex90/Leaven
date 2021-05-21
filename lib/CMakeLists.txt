cmake_minimum_required(VERSION 3.10)
project(LeavenLib)

set(CMAKE_CXX_STANDARD 14)

# DiscreGrid
include_directories(ext/Discregrid/discregrid/include)
include_directories(ext/Discregrid/discregrid/include/Discregrid)
file(GLOB_RECURSE GRID_HEADERS ext/Discregrid/discregrid/include/*.h ext/Discregrid/discregrid/include/*.hpp ext/Discregrid/discregrid/include/*.inl ext/Discregrid/discregrid/include/All)
file(GLOB_RECURSE GRID_SOURCES ext/Discregrid/discregrid/src/*.cpp)

# Require Eigen
find_package(Eigen3 3.3 REQUIRED)

# Find all Header and Source files
file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS src/*.h)
file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES src/*.cpp)

add_library(${PROJECT_NAME}
        ${${PROJECT_NAME}_SOURCES}
        ${${PROJECT_NAME}_HEADERS}
        ${GRID_HEADERS}
        ${GRID_SOURCES})

target_link_libraries(${PROJECT_NAME}
	Eigen3::Eigen)

target_include_directories(${PROJECT_NAME} PUBLIC src)
target_include_directories(${PROJECT_NAME} PUBLIC ext/Discregrid/discregrid/include)