#include "spi_bus.hpp"
#include <iostream>

SPI_Bus::SPI_Bus()
{
    SPI_Bus::m_chip = nullptr;
    SPI_Bus::m_miso = nullptr;
    SPI_Bus::m_mosi = nullptr;
    SPI_Bus::m_sck = nullptr;
    SPI_Bus::m_cs = nullptr;
}

SPI_Bus::~SPI_Bus(){
    release();
}

bool SPI_Bus::init(const char* chipname,
            unsigned int mosi_line, unsigned int miso_line,
            unsigned int sck_line,  unsigned int cs_line,
            unsigned int delay_us)
{
    m_chip = gpiod_chip_open_by_name(chipname);
    if (!m_chip)
        std::cerr << "Failed to open gpiochip\n";

    m_mosi = gpiod_chip_get_line(m_chip, mosi_line);
    m_miso = gpiod_chip_get_line(m_chip, miso_line);
    m_sck  = gpiod_chip_get_line(m_chip, sck_line);
    m_cs   = gpiod_chip_get_line(m_chip, cs_line);
    if (!m_mosi || !m_miso || !m_sck || !m_cs) return false;

    gpiod_line_request_output(m_mosi, "spi_mosi", 1);
    gpiod_line_request_output(m_miso, "spi_miso", 1);
    gpiod_line_request_output(m_sck, "spi_sck", 1);
    gpiod_line_request_output(m_cs, "spi_cs", 1);
    return true;
}

void SPI_Bus::release() {
    if (m_mosi) gpiod_line_release(m_mosi);
    if (m_miso) gpiod_line_release(m_miso);
    if (m_sck)  gpiod_line_release(m_sck);
    if (m_cs)   gpiod_line_release(m_cs);
    if (m_chip) gpiod_chip_close(m_chip);
}

void SPI_Bus::csLow()  { gpiod_line_set_value(m_cs, 0); usleep(m_delayUs); }
void SPI_Bus::csHigh() { gpiod_line_set_value(m_cs, 1); usleep(m_delayUs); }

uint8_t SPI_Bus::transferByte(uint8_t data) {
    uint8_t rx = 0;
    for (int i = 0; i < 8; i++) {
        int bit = (data & 0x80) ? 1 : 0;
        gpiod_line_set_value(m_mosi, bit);
        gpiod_line_set_value(m_sck, 1);
        int rbit = gpiod_line_get_value(m_miso);
        rx = (rx << 1) | (rbit & 1);
        gpiod_line_set_value(m_sck, 0);
        data <<= 1;
        usleep(m_delayUs);
    }
    return rx;
}