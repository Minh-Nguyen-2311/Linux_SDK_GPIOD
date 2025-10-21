#include "uart_device.hpp"
#include <iostream>

int main()
{
    NextionDisplay nextion;
    if(!nextion.init("gpiochip0", 14, 15, 10)){
        std::cout << "Init failed\n";
        return -1;
        goto exit;
    }
    while (1) {
        nextion.clearScreen();
        nextion.setText("t0", "Hello Xenomai!");
        nextion.setBacklight(80);

        std::vector<uint8_t> resp;
        if (nextion.readResponse(resp, 3)) {
            std::cout << "Response: ";
            for (auto b : resp)
            std::cout << std::hex << int(b) << " ";
            std::cout << std::endl;
        }
    }
exit:
    nextion.~NextionDisplay();
    return 0;
}