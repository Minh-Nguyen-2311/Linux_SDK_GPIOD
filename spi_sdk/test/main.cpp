#include "spi_device.hpp"
#include <iostream>

int main()
{
    IMU9250 imu;
    if(!imu.init("gpiochip0", 10, 9, 11, 5, 10)){
        std::cerr << "Init failed\n";
        return -1;
    }
    while (1) {
        float ax, ay, az,
              gx, gy, gz,
              mx, my, mz;
        imu.readAll(ax, ay, az, gx, gy, gz, mx, my, mz);

        std::cout << "A: [" << ax << ", " << ay << ", " << az << "] "
                  << "G: [" << gx << ", " << gy << ", " << gz << "] "
                  << "M: [" << mx << ", " << my << ", " << mz << "]"
                  << std::endl;

        usleep(10000); // 100 Hz
    }
    return 0;
}