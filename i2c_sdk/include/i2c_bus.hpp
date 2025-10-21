#pragma once
#include <gpiod.h>
#include <cstdint>
#include <string>
#include <unistd.h>

class I2C_Bus{
public:
    I2C_Bus();
    ~I2C_Bus();

    virtual bool init(const char* chipname,
            unsigned int sda_line,
            unsigned int scl_line,
            unsigned int delay_us);
    virtual void release();

protected:
    void startCondition();
    void stopCondition();
    bool writeByte(uint8_t data);
    uint8_t readByte(bool ack);
    bool writeRegister(uint8_t addr, uint8_t reg, uint8_t data);
    bool readRegister(uint8_t addr, uint8_t reg, uint8_t& data);

private:
    void sdaHigh();
    void sdaLow();
    void sclHigh();
    void sclLow();
    bool readSDA();
    void delay();

    struct gpiod_chip* m_chip;
    struct gpiod_line* m_sda;
    struct gpiod_line* m_scl;

    unsigned int m_delayUs;
};