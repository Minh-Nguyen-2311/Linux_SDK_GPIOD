cmake_minimum_required(VERSION 3.10)

project(servo_sdk LANGUAGES CXX)

#--Compiler flags--
set(CMAKE_CXX_STANDARD 17) # Set the C++ standard to use (e.g., C++17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE) # Ensure the specified C++ standard is strictly enforced

#--Include headers--
include_directories(include)

# Nếu muốn chắc chắn tìm được, có thể thêm:
include_directories(/usr/include)

#--Static library--
add_library(servo_sdk_lib 
    src/pid.cpp
    src/servo_driver.cpp
)

#--Executable--
add_executable(servo_sdk_exe
    test/main.cpp
)

# --- Link library to executable ---
target_link_libraries(servo_sdk_exe 
PRIVATE 
servo_sdk_lib
gpiod)
