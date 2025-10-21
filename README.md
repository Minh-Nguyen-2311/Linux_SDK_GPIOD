# Linux_SDK_GPIOD

This repo is for my built Linux_SDK for peripheral devices using GPIOD v1.6.5 on Ubuntu 25.10.

Instructions:
1) Install GPIOD from official Linux repo:
   sudo apt update | sudo apt install -y git build-essential autoconf automake libtool pkg-config
2) Git checkout to v1.6.5 of GPIOD:
   git clone https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git
   git checkout v1.6.5
3) Install new GPIOD with CMake:
   ./autogen.sh
   ./configure
   make -j$(nproc)
   sudo make install
4) Install the project with CMake:
   make clean
   cmake .
   make
   ./xx_sdk_exe
