cmake_minimum_required(VERSION 3.10)

project(i2c_sdk LANGUAGES CXX)

#--Compiler flags--
set(CMAKE_CXX_STANDARD 17) # Set the C++ standard to use (e.g., C++17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE) # Ensure the specified C++ standard is strictly enforced

#--Include headers--
include_directories(include)

# Nếu muốn chắc chắn tìm được, có thể thêm:
include_directories(/usr/include)

#--Static library--
add_library(uart_sdk_lib 
    src/uart_bus.cpp
    src/uart_device.cpp
)

#--Executable--
add_executable(uart_sdk_exe
    test/main.cpp
)

# --- Link library to executable ---
target_link_libraries(uart_sdk_exe 
    PRIVATE 
    uart_sdk_lib
    gpiod)