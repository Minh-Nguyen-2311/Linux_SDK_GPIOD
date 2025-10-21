#pragma once
#include <iostream>
#include "i2c_bus.hpp"

class EEPROM24C02 : public I2C_Bus {
public:
    EEPROM24C02(uint8_t addr);
    ~EEPROM24C02();
    bool init(const char* chipname,
            unsigned int sda_line,
            unsigned int scl_line,
            unsigned int delay_us) override;
    void release() override;
    bool writeByte(uint8_t memAddr, uint8_t data);
    bool readByte(uint8_t memAddr, uint8_t& data);
    void printInfo();
protected:
    uint8_t m_addr;
};