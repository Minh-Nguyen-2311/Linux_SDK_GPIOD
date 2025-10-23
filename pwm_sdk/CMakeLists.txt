cmake_minimum_required(VERSION 3.10)

project(pwm_sdk LANGUAGES CXX)

#--Compiler flags--
set(CMAKE_CXX_STANDARD 17) # Set the C++ standard to use (e.g., C++17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE) # Ensure the specified C++ standard is strictly enforced

#--Include headers--
include_directories(include)

#--Include gpiod header--
include_directories(/usr/include)

#--Static library--
add_library(pwm_sdk_lib 
    src/pwm_bus.cpp
    src/servo_180.cpp
    src/servo_360.cpp
)

#--Executable--
add_executable(pwm_sdk_exe
    test/main.cpp
)

# --- Link library to executable ---
target_link_libraries(pwm_sdk_exe 
PRIVATE 
pwm_sdk_lib
gpiod)
