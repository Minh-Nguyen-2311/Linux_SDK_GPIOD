#include "spi_device.hpp"
#include <iostream>

// MPU-9250 Register Map
#define MPU_READ_FLAG     0x80
#define WHO_AM_I_MPU9250  0x75
#define PWR_MGMT_1        0x6B
#define ACCEL_XOUT_H      0x3B
#define GYRO_XOUT_H       0x43
#define USER_CTRL         0x6A
#define INT_PIN_CFG       0x37
#define I2C_MST_CTRL      0x24
#define AK8963_CNTL1      0x0A
#define AK8963_ST1        0x02
#define AK8963_XOUT_L     0x03
#define AK8963_WHO_AM_I   0x00

IMU9250::IMU9250()
{
    IMU9250::m_accelScale = 0.0f;
    IMU9250::m_gyroScale = 0.0f;
    IMU9250::m_magScale = 0.0f;
}

IMU9250::~IMU9250(){
    IMU9250::release();
}

bool IMU9250::init(const char* chipname,
            unsigned int mosi_line, unsigned int miso_line,
            unsigned int sck_line,  unsigned int cs_line,
            unsigned int delay_us)
{
    // Gọi init() của lớp cha để setup GPIO & bus
    if(!SPI_Bus::init(chipname, mosi_line, miso_line, sck_line, cs_line, delay_us))
    {
        std::cerr << "IMU9250 init: failed to init SPI bus\n";
        return false;
    }

    csLow();
    transferByte(WHO_AM_I_MPU9250 | MPU_READ_FLAG);
    uint8_t whoami = transferByte(0x00);
    csHigh();

    if (whoami != 0x71 && whoami != 0x73) {
        std::cerr << "IMU9250 not detected (WHO_AM_I=" << int(whoami) << ")\n";
        return false;
    }

    // Wake up
    writeReg(PWR_MGMT_1, 0x00);
    usleep(10000);

    configureAccel();
    configureGyro();

    std::cout << "MPU-9250 initialized (WHO_AM_I=" << int(whoami) << ")\n";
    return true;
}

void IMU9250::release()
{
    SPI_Bus::release();
}

void IMU9250::writeReg(uint8_t reg, uint8_t val) {
    csLow();
    transferByte(reg & 0x7F);
    transferByte(val);
    csHigh();
    usleep(100);
}

uint8_t IMU9250::readReg(uint8_t reg) {
    csLow();
    transferByte(reg | MPU_READ_FLAG);
    uint8_t val = transferByte(0x00);
    csHigh();
    return val;
}

void IMU9250::readRegs(uint8_t reg, uint8_t* buf, size_t len) {
    csLow();
    transferByte(reg | MPU_READ_FLAG);
    for (size_t i = 0; i < len; i++)
        buf[i] = transferByte(0x00);
    csHigh();
}

void IMU9250::configureAccel() {
    // ±2g range
    writeReg(0x1C, 0x00);
    m_accelScale = 2.0f / 32768.0f;
}

void IMU9250::configureGyro() {
    // ±250°/s
    writeReg(0x1B, 0x00);
    m_gyroScale = 250.0f / 32768.0f;
}

void IMU9250::configureMag() {
    // Enable bypass for AK8963 I²C (optional)
    writeReg(INT_PIN_CFG, 0x22);
    writeReg(USER_CTRL, 0x00);
    usleep(10000);
}

void IMU9250::readAccel(float& ax, float& ay, float& az) {
    uint8_t buf[6];
    readRegs(ACCEL_XOUT_H, buf, 6);
    int16_t x = (buf[0] << 8) | buf[1];
    int16_t y = (buf[2] << 8) | buf[3];
    int16_t z = (buf[4] << 8) | buf[5];
    ax = x * m_accelScale;
    ay = y * m_accelScale;
    az = z * m_accelScale;
}

void IMU9250::readGyro(float& gx, float& gy, float& gz) {
    uint8_t buf[6];
    readRegs(GYRO_XOUT_H, buf, 6);
    int16_t x = (buf[0] << 8) | buf[1];
    int16_t y = (buf[2] << 8) | buf[3];
    int16_t z = (buf[4] << 8) | buf[5];
    gx = x * m_gyroScale;
    gy = y * m_gyroScale;
    gz = z * m_gyroScale;
}

void IMU9250::readMag(float& mx, float& my, float& mz) {
    // Simplified — only valid if AK8963 in bypass mode
    uint8_t buf[6];
    readRegs(AK8963_XOUT_L | MPU_READ_FLAG, buf, 6);
    int16_t x = (buf[1] << 8) | buf[0];
    int16_t y = (buf[3] << 8) | buf[2];
    int16_t z = (buf[5] << 8) | buf[4];
    mx = x * m_magScale;
    my = y * m_magScale;
    mz = z * m_magScale;
}

void IMU9250::readAll(float& ax, float& ay, float& az,
                      float& gx, float& gy, float& gz,
                      float& mx, float& my, float& mz) {
    readAccel(ax, ay, az);
    readGyro(gx, gy, gz);
    readMag(mx, my, mz);
}
