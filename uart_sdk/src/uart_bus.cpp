#include "uart_bus.hpp"
#include <iostream>

UART_Bus::UART_Bus()
{
    UART_Bus::m_chip = nullptr;
    UART_Bus::m_tx = nullptr;
    UART_Bus::m_rx = nullptr;
}

UART_Bus::~UART_Bus()
{
    release();
}

bool UART_Bus::init(const char* chipname,
            unsigned int tx_line, unsigned int rx_line,
            unsigned int delay_us)
{
    int tx_out, rx_out;
    m_chip = gpiod_chip_open_by_name(chipname);

    if (!m_chip)
        std::cout << "Failed to open gpiochip\n";

    m_tx = gpiod_chip_get_line(m_chip, tx_line);
    m_rx = gpiod_chip_get_line(m_chip, rx_line);
    if (!m_tx || !m_rx) {
        std::cout << "UART GPIO fail\n";
        return false;
    }

    tx_out = gpiod_line_request_output(m_tx, "uart_tx", 1);
    rx_out = gpiod_line_request_output(m_rx, "uart_rx", 1);
    if(!tx_out || !rx_out){
        std::cout << "UART GPIO request output fail\n";
        return false;
    }
    return true;
}

void UART_Bus::release()
{
    if (m_tx) gpiod_line_release(m_tx);
    if (m_rx) gpiod_line_release(m_rx);
}

uint8_t UART_Bus::sendByte(uint8_t data)
{
    if (!m_tx) {
        std::cerr << "[WARN] TX line not initialized\n";
        return 0;
    }

    // Start bit (LOW)
    gpiod_line_set_value(m_tx, 0);
    usleep(m_delayUs);

    // 8 data bits (LSB first)
    for (int i = 0; i < 8; i++) {
        int bit = (data >> i) & 0x01;
        gpiod_line_set_value(m_tx, bit);
        usleep(m_delayUs);
    }

    // Stop bit (HIGH)
    gpiod_line_set_value(m_tx, 1);
    usleep(m_delayUs);

    return 1; // success
}


uint8_t UART_Bus::receivedByte(uint8_t* data)
{
    if (!m_rx) {
        std::cerr << "[WARN] RX line not initialized\n";
        return 0;
    }

    // Đợi start bit (LOW)
    int val;
    do {
        val = gpiod_line_get_value(m_rx);
        if (val < 0) return 0; // lỗi đọc
    } while (val == 1);

    // Đợi nửa bit để vào giữa bit start
    usleep(m_delayUs / 2);

    uint8_t value = 0;

    for (int i = 0; i < 8; i++) {
        usleep(m_delayUs);
        val = gpiod_line_get_value(m_rx);
        if (val < 0) return 0;
        value |= (val << i);
    }

    // Bỏ qua stop bit
    usleep(m_delayUs);

    *data = value;
    return 1;
}