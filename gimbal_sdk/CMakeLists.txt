cmake_minimum_required(VERSION 3.10)

project(gimbal_sdk LANGUAGES CXX)

#--Compiler flags--
set(CMAKE_CXX_STANDARD 17) # Set the C++ standard to use (e.g., C++17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE) # Ensure the specified C++ standard is strictly enforced

#--Include headers--
include_directories(include)

# Nếu muốn chắc chắn tìm được, có thể thêm:
include_directories(/usr/include)

find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})

include_directories(../vision_sdk/include
                    ../pwm_sdk/include)

#--Static library--
add_library(gimbal_sdk_lib 
    src/gimbal_controller.cpp
)

#--Executable--
add_executable(gimbal_sdk_exe
    test/main.cpp
)

# --- Link library to executable ---
target_link_libraries(gimbal_sdk_exe 
    ${OpenCV_LIBS} 
    gimbal_sdk_lib
    ${CMAKE_SOURCE_DIR}/libpwm_sdk_lib.a
    ${CMAKE_SOURCE_DIR}/libvision_sdk_lib.a
    opencv_tracking
    gpiod)
