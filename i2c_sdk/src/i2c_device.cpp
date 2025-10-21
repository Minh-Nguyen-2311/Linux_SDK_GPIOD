#include "i2c_device.hpp"

EEPROM24C02::EEPROM24C02(uint8_t addr)
    : m_addr(addr) {}

EEPROM24C02::~EEPROM24C02(){
    EEPROM24C02::release();
}

bool EEPROM24C02::init(const char* chipname,
                        unsigned int sda_line,
                        unsigned int scl_line,
                        unsigned int delay_us)
{
    // Gọi init() của lớp cha để setup GPIO & bus
    if (!I2C_Bus::init(chipname, sda_line, scl_line, delay_us)) {
        std::cout << "EEPROM init: failed to init I2C bus\n";
        return false;
    }

    // Kiểm tra thiết bị có phản hồi không
    uint8_t test_byte = 0;
    bool ok = readRegister(m_addr, 0x00, test_byte);
    if (!ok) std::cout << "EEPROM not responding at address 0x" << std::hex << int(m_addr) << "\n";

    std::cout << "Init EEPROM okay\n";
    return ok;
}

void EEPROM24C02::release(){
    I2C_Bus::release();
}

bool EEPROM24C02::writeByte(uint8_t memAddr, uint8_t data) {
    return writeRegister(m_addr, memAddr, data);
}

bool EEPROM24C02::readByte(uint8_t memAddr, uint8_t& data) {
    return readRegister(m_addr, memAddr, data);
}

void EEPROM24C02::printInfo(){
    std::cout << "EEPROM 24C02 @ I2C address 0x"
              << std::hex << int(m_addr)
              << std::dec << std::endl;
}