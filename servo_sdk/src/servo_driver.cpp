#include "servo_driver.hpp"
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>

static struct gpiod_chip *chip;
static struct gpiod_line *line;

int servo_driver_init(const char *chipname, unsigned int line_num)
{
    chip = gpiod_chip_open_by_name(chipname);
    if (!chip) {
        std::cerr << "Could not open chip" << std::endl;
        return -1;
    }

    line = gpiod_chip_get_line(chip, line_num);
    if(!line){
        std::cerr << "Could not get line (GPIO)" << std::endl;
        return -2;
    }

    if(gpiod_line_request_output(line, "servo_driver", 0) < 0){
        std::cerr << "Could not request output" << std::endl;
        return -3;
    }

    //std::cout << "Servo driver initialized on " << chipname << "on line " << line_num;
    return 0;
}

void servo_driver_set_angle(float angle)
{
    if (!line) {
        std::cerr << "Servo driver not initialized!\n";
        return;
    }

    // Giới hạn góc hợp lệ
    if (angle < 0.0f) angle = 0.0f;
    if (angle > 180.0f) angle = 180.0f;

    // Servo tiêu chuẩn: 1ms (0°) → 2ms (180°)
    float duty_min = 0.5f;   // ms
    float duty_max = 2.5f;   // ms
    float duty_ms = duty_min + (angle / 180.0f) * (duty_max - duty_min);

    // Chu kỳ PWM = 20ms
    float period_ms = 1000.0f / 50;

    // Tính toán tỉ lệ thời gian bật/tắt
    auto high_time = std::chrono::microseconds((int)(duty_ms * 1000));
    auto low_time = std::chrono::microseconds((int)((period_ms - duty_ms) * 1000));

    // Gửi 20 xung PWM để servo cập nhật vị trí
    for (int i = 0; i < 20; ++i) {
        gpiod_line_set_value(line, 1);
        std::this_thread::sleep_for(high_time);
        gpiod_line_set_value(line, 0);
        std::this_thread::sleep_for(low_time);
    }

    std::cout << "Moved to angle: " << angle << " degrees" << std::endl;
}

// -1: Max CW, 1: Max CCW
void servo_driver_set_direction(float speed)
{
    int high_us = 1500 + (speed * 500);
    int low_us  = 20000 - high_us;

    gpiod_line_set_value(line, 1);
    usleep(high_us);
    gpiod_line_set_value(line, 0);
    usleep(low_us);
}

void servo_driver_stop(void)
{
    if (line) gpiod_line_release(line);
    if (chip) gpiod_chip_close(chip);
}
