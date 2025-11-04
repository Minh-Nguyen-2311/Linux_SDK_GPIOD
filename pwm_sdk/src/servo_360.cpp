#include "servo_360.hpp"
#include <cmath>
#include <thread>
#include <atomic>
#include <algorithm>
#include <iostream>

// ===== file-scope state for feedback thread =====
static std::thread        g_fb_thread;
static std::atomic<bool>  g_fb_running{false};
static std::atomic<float> g_latest_angle{0.0f};    // 0..360 (filtered)
static std::atomic<bool>  g_has_sample{false};

// PID state (file-scope for simplicity; per-process single servo)
static float s_prev_err = 0.0f;
static float s_integral = 0.0f;
static float s_target_pulse = 1500.0f; // microseconds

Servo360::Servo360(){}

Servo360::~Servo360()
{
    Servo360::release(); 
}

bool Servo360::init(const char* chipname,
                    unsigned int pwm0_line,
                    unsigned int pwm1_line)
{
    if (!PWM_Bus::init(chipname, pwm0_line, pwm1_line)) {
        std::cerr << "Parallax Servo failed to initialize PWM bus\n";
        return false;
    }
    std::cout << "Parallax Servo initialized (PWM_Out=" << pwm0_line
              << ", PWM_In=" << pwm1_line << ")\n";

    // Start feedback thread
    g_fb_running.store(true, std::memory_order_relaxed);
    g_fb_thread = std::thread([this]() {
        while (g_fb_running.load(std::memory_order_relaxed)) {
            float ang = this->readFeedbackAngle();   // blocking waiting edges
            g_latest_angle.store(ang, std::memory_order_relaxed);
            g_has_sample.store(true, std::memory_order_relaxed);
        }
    });

    // start PWM output at neutral
    PWM_Bus::PWM_Out(1500.0f);
    return true;
}

void Servo360::release()
{
    g_fb_running.store(false);
    if (g_fb_thread.joinable()) g_fb_thread.join();
    PWM_Bus::release();
}

// Blocking: wait edges R-F-R and compute filtered 0..360 angle
float Servo360::readFeedbackAngle() {
    struct gpiod_line_event evt;
    struct timespec ts_rise1{}, ts_fall{}, ts_rise2{};

    // RISING 1
    while (true) {
        if (gpiod_line_event_wait(m_pwm1, nullptr) != 1) continue;
        if (!gpiod_line_event_read(m_pwm1, &evt) && evt.event_type == GPIOD_LINE_EVENT_RISING_EDGE) {
            ts_rise1 = evt.ts; break;
        }
    }
    // FALLING
    while (true) {
        if (gpiod_line_event_wait(m_pwm1, nullptr) != 1) continue;
        if (!gpiod_line_event_read(m_pwm1, &evt) && evt.event_type == GPIOD_LINE_EVENT_FALLING_EDGE) {
            ts_fall = evt.ts; break;
        }
    }
    // RISING 2
    while (true) {
        if (gpiod_line_event_wait(m_pwm1, nullptr) != 1) continue;
        if (!gpiod_line_event_read(m_pwm1, &evt) && evt.event_type == GPIOD_LINE_EVENT_RISING_EDGE) {
            ts_rise2 = evt.ts; break;
        }
    }

    auto to_ms = [](const timespec& t) -> double {
        return (double)t.tv_sec * 1e3 + (double)t.tv_nsec / 1e6;
    };

    double rise1 = to_ms(ts_rise1);
    double fall  = to_ms(ts_fall);
    double rise2 = to_ms(ts_rise2);

    double highTime  = std::clamp(fall - rise1,  MIN_T_HIGH_MS,   MAX_T_HIGH_MS);
    double cycleTime = std::clamp(rise2 - rise1, MIN_FEEDBACK_MS, MAX_FEEDBACK_MS);

    // If cycle looks insane, return last sample
    if (cycleTime <= 0.5 || cycleTime >= 2.0) {
        return g_latest_angle.load(std::memory_order_relaxed);
    }

    double duty = std::clamp((highTime / cycleTime) * 100.0, MIN_DUTY, MAX_DUTY);
    float angle_raw = (float)((duty - MIN_DUTY) * 360.0 / (MAX_DUTY - MIN_DUTY));

    // Low-pass filter
    static float filtered = 0.0f;
    filtered = ALPHA * angle_raw + (1.0f - ALPHA) * filtered;

    return filtered;  // 0..360 filtered
}

// Compute PID from latest sample (do NOT block here)
void Servo360::updatePIDWithFeedback(float target_deg, float dt)
{
    if (!g_has_sample.load(std::memory_order_relaxed)) {
        // no sample yet, keep neutral
        PWM_Bus::PWM_Out(1500.0f);
        return;
    }

    float angle = g_latest_angle.load(std::memory_order_relaxed);

    // wrap target to [0..360)
    float target = std::fmod(target_deg, 360.0f);
    if (target < 0) target += 360.0f;

    // error in [-180..180]
    float err = target_deg - angle;
    if (err > 180.0f)  err -= 360.0f;
    if (err < -180.0f) err += 360.0f;

    // PID with anti-windup
    s_integral += err * dt;
    const float i_max = 50.0f;      // guard
    if (s_integral > i_max)  s_integral = i_max;
    if (s_integral < -i_max) s_integral = -i_max;

    float deriv = (err - s_prev_err) / dt;
    s_prev_err = err;

    float u = KP * err + KI * s_integral + KD * deriv;
    // map to pulse around neutral. You can tune 220 → 200..250
    float pulse_us = 1500.0f + std::clamp(u, -1.0f, 1.0f) * 220.0f;

    // soft slew rate limit ~40us/step at 60Hz
    float max_slew = 40.0f;
    float delta = pulse_us - s_target_pulse;
    if (delta >  max_slew) delta =  max_slew;
    if (delta < -max_slew) delta = -max_slew;
    s_target_pulse += delta;

    // push to PWM generator (non-blocking)
    PWM_Bus::PWM_Out(s_target_pulse);

    std::cout << "[PID] target=" << target_deg
              << " actual=" << angle
              << " err=" << err
              << " pulse_us=" << s_target_pulse
              << std::endl;
}
