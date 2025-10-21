#pragma once
#include <gpiod.h>
#include <cstdint>
#include <string>
#include <unistd.h>

class UART_Bus{
public:
    UART_Bus();
    ~UART_Bus();

    virtual bool init(const char* chipname,
            unsigned int tx_line, unsigned int rx_line,
            unsigned int delay_us);
    virtual void release();

protected:
    uint8_t sendByte(uint8_t data);
    uint8_t receivedByte(uint8_t* data);
    
private:
    struct gpiod_chip* m_chip;
    
    struct gpiod_line* m_tx;
    struct gpiod_line* m_rx;

    unsigned int m_delayUs;
};