#pragma once
#include <gpiod.h>
#include <cstdint>
#include <string>
#include <unistd.h>

class SPI_Bus{
public:
    SPI_Bus();
    ~SPI_Bus();

    virtual bool init(const char* chipname,
            unsigned int mosi_line, unsigned int miso_line,
            unsigned int sck_line,  unsigned int cs_line,
            unsigned int delay_us);
    virtual void release();

protected:
    void csLow();
    void csHigh();
    uint8_t transferByte(uint8_t data);
    
private:
    struct gpiod_chip* m_chip;
    
    struct gpiod_line* m_mosi;
    struct gpiod_line* m_miso;
    struct gpiod_line* m_sck;
    struct gpiod_line* m_cs;

    unsigned int m_delayUs;
};