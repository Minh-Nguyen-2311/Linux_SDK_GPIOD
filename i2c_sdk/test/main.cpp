#include "i2c_device.hpp"
#include <iostream>

int main() {
    EEPROM24C02 eeprom(0x50);

    if (!eeprom.init("gpiochip0", 2, 3, 10)) {
        std::cerr << "Init failed\n";
        return -1;
        goto exit;
    }

    while(1)
    {
        eeprom.printInfo();

        // Write one byte
        eeprom.writeByte(0x00, 0xAB);

        // Read it back
        uint8_t val = 0;
        eeprom.readByte(0x00, val);
        std::cout << "Read value: 0x" << std::hex << int(val) << std::endl;
    }

exit:
    eeprom.release();
    return 0;
}