#include "servo_180.hpp"
#include "servo_360.hpp"
#include <iostream>
#include <time.h>

// Fixed-period sleep using absolute MONOTONIC time
static void sleep_until_next(struct timespec& next, const long period_ns)
{
    next.tv_nsec += period_ns;
    while (next.tv_nsec >= 1000000000L) { next.tv_nsec -= 1000000000L; next.tv_sec++; }
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, nullptr);
}

int main()
{
    std::cout << "Testing Parallax 360 servo (2-thread model)\n";

    Servo360 servo360;
    if (!servo360.init("gpiochip0", 14, 12)) {
        std::cerr << "Init failed\n";
        return -1;
    }

    float target_deg;
    std::cout << "Enter desired angle: ";
    std::cin >> target_deg;

    // Control loop at .. Hz
    const double dt = 1.0 / 60.0;
    const long period_ns = 16'666'667L;

    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    while (true)
    {
        float current_deg = servo360.readFeedbackAngle(); 
        float error = target_deg - current_deg;

        if(error > 180.0f) error -= 360.0f;
        else if(error < -180.0f) error += 360.0f;

        servo360.updatePIDWithFeedback(target_deg, (float)dt);

        if (std::abs(error) < 1.0f) {
            std::cout << "Reached target!\n";
            std::cout << "Final angle: " << current_deg << "\n";
            break;
        }

        sleep_until_next(next, period_ns);
    }

    servo360.release();
    std::cout << "Done.\n";
    return 0;
}

