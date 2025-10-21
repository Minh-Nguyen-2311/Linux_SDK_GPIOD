#include "uart_device.hpp"
#include <iostream>
#include <thread>
#include <chrono>

NextionDisplay::NextionDisplay(){
    
}

NextionDisplay::~NextionDisplay(){
    release(); 
}

void NextionDisplay::release()
{
    UART_Bus::release();
}

bool NextionDisplay::init(const char* chipname,
                          unsigned int tx_line, unsigned int rx_line,
                          unsigned int delay_us)
{
    if (!UART_Bus::init(chipname, tx_line, rx_line, delay_us)) {
        std::cerr << "Nextion init: failed to initialize UART bus\n";
        return false;
    }

    std::cout << " Nextion display UART initialized (TX=" << tx_line
              << ", RX=" << rx_line << ")\n";
    return true;
}

void NextionDisplay::sendCommand(const std::string& cmd)
{
    // Gửi từng byte trong chuỗi
    for (auto c : cmd)
        sendByte(static_cast<uint8_t>(c));

    // Gửi chuỗi kết thúc 0xFF 0xFF 0xFF theo chuẩn Nextion
    sendTerminator();
}

bool NextionDisplay::readResponse(std::vector<uint8_t>& response, size_t expected_len)
{
    response.clear();
    for (size_t i = 0; i < expected_len; ++i) {
        uint8_t data = 0;
        if (receivedByte(&data))
            response.push_back(data);
        else
            return false;
    }
    return true;
}

void NextionDisplay::sendTerminator()
{
    sendByte(0xFF);
    sendByte(0xFF);
    sendByte(0xFF);
}

void NextionDisplay::clearScreen()
{
    sendCommand("cls BLACK");
}

void NextionDisplay::setText(const std::string& component, const std::string& text)
{
    std::string cmd = component + ".txt=\"" + text + "\"";
    sendCommand(cmd);
}

void NextionDisplay::setBacklight(uint8_t level)
{
    if (level > 100) level = 100;
    std::string cmd = "dim=" + std::to_string(level);
    sendCommand(cmd);
}
