#include "pwm_bus.hpp"
#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <cmath>

// ====== PWM background generator (50 Hz) ======
static std::thread        g_pwm_thread;
static std::atomic<bool>  g_pwm_running{false};
static std::atomic<float> g_pulse_us{1500.0f};   // desired pulse width in microseconds

// Helpers
static inline uint64_t us_now()
{
    using namespace std::chrono;
    return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

PWM_Bus::PWM_Bus()
{
    PWM_Bus::m_chip = nullptr;
    PWM_Bus::m_pwm0 = nullptr;
    PWM_Bus::m_pwm1 = nullptr;
}

PWM_Bus::~PWM_Bus()
{
    release();
}

bool PWM_Bus::init(const char* chipname,
                   unsigned int pwm0_line,
                   unsigned int pwm1_line)
{
    m_chip = gpiod_chip_open_by_name(chipname);
    if (!m_chip) {
        std::cout << "Failed to open gpiochip\n";
        return false;
    }

    m_pwm0 = gpiod_chip_get_line(m_chip, pwm0_line); // output to servo
    m_pwm1 = gpiod_chip_get_line(m_chip, pwm1_line); // input feedback
    if (!m_pwm0 || !m_pwm1) {
        std::cout << "PWM GPIO get line fail\n";
        return false;
    }

    if (gpiod_line_request_output(m_pwm0, "pwm_out", 0) < 0) {
        std::cout << "PWM GPIO request output fail\n";
        return false;
    }

    if (gpiod_line_request_both_edges_events(m_pwm1, "pwm_in") < 0) {
        std::cout << "PWM GPIO request edge events fail\n";
        return false;
    }

    // Start background PWM at 50 Hz, using last set g_pulse_us
    g_pwm_running.store(true, std::memory_order_relaxed);
    g_pwm_thread = std::thread([this]() {
        using namespace std::chrono;
        const int period_us = 20000; // 50 Hz
        // Try to keep absolute schedule to reduce drift
        uint64_t next_tick = us_now();

        while (g_pwm_running.load(std::memory_order_relaxed)) {
            float pulse = g_pulse_us.load(std::memory_order_relaxed);
            // clamp safe range ~0.3ms..2.8ms
            if (pulse < 300.0f)  pulse = 300.0f;
            if (pulse > 2800.0f) pulse = 2800.0f;

            gpiod_line_set_value(m_pwm0, 1);
            std::this_thread::sleep_for(microseconds((int)pulse));
            gpiod_line_set_value(m_pwm0, 0);

            // sleep the rest of the 20ms period
            int rest = period_us - (int)pulse;
            if (rest > 0) std::this_thread::sleep_for(microseconds(rest));

            // keep absolute timing if needed (optional)
            next_tick += period_us;
        }
        // ensure low on exit
        gpiod_line_set_value(m_pwm0, 0);
    });

    return true;
}

void PWM_Bus::release()
{
    // stop PWM thread
    if (g_pwm_running.load()) {
        g_pwm_running.store(false);
        if (g_pwm_thread.joinable()) g_pwm_thread.join();
    }

    if (m_pwm0) gpiod_line_release(m_pwm0);
    if (m_pwm1) gpiod_line_release(m_pwm1);
    if (m_chip) gpiod_chip_close(m_chip);

    m_chip = nullptr;
    m_pwm0 = m_pwm1 = nullptr;
}

// Backward-compatible API but NON-BLOCKING now.
// Accepts:
//  - duty in fraction (0..1)  -> converted to microseconds of a 20ms frame
//  - duty in milliseconds (1..3) -> treated as ms
//  - duty in microseconds (>100) -> treated as µs
void PWM_Bus::PWM_Out(float duty)
{
    const float period_us = 20000.0f;

    float pulse_us;
    if (duty <= 1.0f) {
        // fraction case (e.g. 1500/20000)
        pulse_us = duty * period_us;
    } else if (duty < 10.0f) {
        // milliseconds case (1.0..2.5 ms)
        pulse_us = duty * 1000.0f;
    } else {
        // microseconds case (typical for control thread: 1300..1700)
        pulse_us = duty;
    }

    // just update the target; background thread outputs continuously
    g_pulse_us.store(pulse_us, std::memory_order_relaxed);
}
