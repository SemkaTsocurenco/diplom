cmake_minimum_required(VERSION 3.10)

project(get_img)

set(CMAKE_CXX_FLAGS "-fPIC -O3 -ffast-math -msse3 -mfpmath=sse")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(Eigen3 3.3 REQUIRED)
find_package(OpenCV 4.5.1 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(EIGEN3_INCLUDE_DIR)


# Основная программа
add_executable(get_img ./main.cpp )

# Линковка библиотек
target_link_libraries(get_img ${OpenCV_LIBS})
target_link_libraries(get_img Eigen3::Eigen)