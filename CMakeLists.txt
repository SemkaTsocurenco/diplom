cmake_minimum_required(VERSION 3.10)

project(diplom)



set(CMAKE_CXX_FLAGS "-fPIC -O3 -ffast-math -msse3 -mfpmath=sse")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(Eigen3 3.3 REQUIRED)
find_package(OpenCV 4.5.1 REQUIRED)

foreach(dep math;random;gui;maps)
    if (NOT TARGET mrpt::${dep})
        find_package(mrpt-${dep} REQUIRED)
    endif()
endforeach()
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories( EIGEN3_INCLUDE_DIR )

add_library(HSV SHARED libs/HSV.cpp)
add_library(hough SHARED libs/hough.cpp)
add_library(canny SHARED libs/canny.cpp)
add_library(ransac SHARED libs/ransac.cpp)
add_library(unet SHARED libs/unet.cpp)
add_library(optic_flow SHARED libs/optic_flow.cpp)
add_library(adaptive_binarization SHARED libs/adaptive_binarization.cpp)




# Основная программа
add_executable(diplom ./main.cpp
                    ./libs/HSV.cpp
)
set(PACKAGE_STRING diplom)

target_link_libraries(diplom dl)
target_link_libraries(diplom ${OpenCV_LIBS} )
target_link_libraries (diplom Eigen3::Eigen)


foreach(dep math;random;gui;maps)
    target_link_libraries(diplom mrpt::${dep})
endforeach()




