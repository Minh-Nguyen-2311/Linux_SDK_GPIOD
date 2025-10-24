cmake_minimum_required(VERSION 3.10)
project(vision_sdk)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})

add_library(vision_sdk_lib
    src/camera_manager.cpp
    src/object_detector.cpp
    src/object_tracker.cpp
    src/vision_pipeline.cpp
)

add_executable(vision_sdk_exe 
    test/main.cpp
)
target_link_libraries(vision_sdk_exe vision_sdk_lib ${OpenCV_LIBS})
