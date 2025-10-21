#include "i2c_bus.hpp"
#include <iostream>

I2C_Bus::I2C_Bus()
{
    I2C_Bus::m_chip = nullptr;
    I2C_Bus::m_sda = nullptr;
    I2C_Bus::m_scl = nullptr;
}

I2C_Bus::~I2C_Bus() { release(); }

bool I2C_Bus::init(const char* chipname,
                    unsigned int sda_line,
                    unsigned int scl_line,
                    unsigned int delay_us) 
{
    int sda_out, scl_out; 
    m_chip = gpiod_chip_open_by_name(chipname);
    if (!m_chip){
        std::cout << "Open chip fail\n";
        return false;
    } 

    m_sda = gpiod_chip_get_line(m_chip, sda_line);
    m_scl = gpiod_chip_get_line(m_chip, scl_line);
    if (!m_sda || !m_scl) {
        std::cout << "I2C get line fail\n";
        return false;
    }

    sda_out = gpiod_line_request_output(m_sda, "i2c_sda", 1);
    scl_out = gpiod_line_request_output(m_scl, "i2c_scl", 1);
    if(!sda_out || !scl_out){
        std::cout << "I2C request output fail\n";
        return false;
    }
    return true;
}

void I2C_Bus::release() {
    if (m_sda) gpiod_line_release(m_sda);
    if (m_scl) gpiod_line_release(m_scl);
    if (m_chip) gpiod_chip_close(m_chip);
    I2C_Bus::m_chip = nullptr;
    I2C_Bus::m_sda = nullptr;
    I2C_Bus::m_scl = nullptr;
}

void I2C_Bus::delay() { usleep(m_delayUs); }

void I2C_Bus::sdaHigh() { gpiod_line_set_value(m_sda, 1); delay(); }
void I2C_Bus::sdaLow()  { gpiod_line_set_value(m_sda, 0); delay(); }
void I2C_Bus::sclHigh() { gpiod_line_set_value(m_scl, 1); delay(); }
void I2C_Bus::sclLow()  { gpiod_line_set_value(m_scl, 0); delay(); }

bool I2C_Bus::readSDA() {
    gpiod_line_request_input(m_sda, "i2c_sda_in");
    bool val = gpiod_line_get_value(m_sda);
    gpiod_line_request_output(m_sda, "i2c_sda_out", 1);
    return val;
}

// --- I2C basic sequence ---
void I2C_Bus::startCondition() {
    sdaHigh(); sclHigh(); sdaLow(); sclLow();
}

void I2C_Bus::stopCondition() {
    sdaLow(); sclHigh(); sdaHigh();
}

bool I2C_Bus::writeByte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        (data & 0x80) ? sdaHigh() : sdaLow();
        sclHigh(); sclLow();
        data <<= 1;
    }

    // ACK
    sdaHigh();
    gpiod_line_request_input(m_sda, "i2c_ack");
    sclHigh();
    bool ack = !gpiod_line_get_value(m_sda);
    sclLow();
    gpiod_line_request_output(m_sda, "i2c_sda", 1);
    return ack;
}

uint8_t I2C_Bus::readByte(bool ack) {
    uint8_t data = 0;
    gpiod_line_request_input(m_sda, "i2c_read");

    for (int i = 0; i < 8; i++) {
        sclHigh();
        data = (data << 1) | gpiod_line_get_value(m_sda);
        sclLow();
    }

    gpiod_line_request_output(m_sda, "i2c_write", 1);
    if (ack) sdaLow(); else sdaHigh();
    sclHigh(); sclLow(); sdaHigh();
    return data;
}

bool I2C_Bus::writeRegister(uint8_t addr, uint8_t reg, uint8_t data) {
    startCondition();
    if (!writeByte(addr << 1)) return false;
    writeByte(reg);
    writeByte(data);
    stopCondition();
    return true;
}

bool I2C_Bus::readRegister(uint8_t addr, uint8_t reg, uint8_t& data) {
    startCondition();
    if (!writeByte(addr << 1)) return false;
    writeByte(reg);
    startCondition();
    writeByte((addr << 1) | 1);
    data = readByte(false);
    stopCondition();
    return true;
}