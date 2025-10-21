#pragma once
#include "spi_bus.hpp"
#include <iostream>

class IMU9250 : public SPI_Bus{
public:
    IMU9250();
    ~IMU9250();
    bool init(const char* chipname,
            unsigned int mosi_line, unsigned int miso_line,
            unsigned int sck_line,  unsigned int cs_line,
            unsigned int delay_us) override;
    void release() override;
    // Accelerometer
    void readAccel(float& ax, float& ay, float& az);

    // Gyroscope
    void readGyro(float& gx, float& gy, float& gz);

    // Magnetometer (AK8963)
    void readMag(float& mx, float& my, float& mz);

    // Combined
    void readAll(float& ax, float& ay, float& az,
                 float& gx, float& gy, float& gz,
                 float& mx, float& my, float& mz);
private:
    // Register helpers
    uint8_t readReg(uint8_t reg);
    void writeReg(uint8_t reg, uint8_t val);
    void readRegs(uint8_t reg, uint8_t* buf, size_t len);

    void configureAccel();
    void configureGyro();
    void configureMag();

    // Conversion scale factors
    float m_accelScale, m_gyroScale, m_magScale;
};