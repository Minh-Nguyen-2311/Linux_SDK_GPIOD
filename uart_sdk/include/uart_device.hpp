#pragma once
#include "uart_bus.hpp"
#include <string>
#include <vector>
#include <iostream>

class NextionDisplay : public UART_Bus {
public:
    NextionDisplay();
    ~NextionDisplay();

    bool init(const char* chipname,
              unsigned int tx_line, unsigned int rx_line,
              unsigned int delay_us) override;

    void release() override;

    void sendCommand(const std::string& cmd);
    bool readResponse(std::vector<uint8_t>& response, size_t expected_len = 3);

    void clearScreen();
    void setText(const std::string& component, const std::string& text);
    void setBacklight(uint8_t level);

private:
    void sendTerminator(); // gửi 0xFF 0xFF 0xFF
};
