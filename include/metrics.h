#pragma once

#include <Arduino.h>


/**
 * @brief Unified telemetry data model for the bike computer.
 * Represents the single source of truth passed to UI and loggers.
 */
struct RideMetrics
{
    // Instantaneous readings
    float speed_kph = 0.0f;
    uint16_t cadence_rpm = 0;
    uint8_t heart_rate_bpm = 0;
    float grade_percent = 0.0f;
    float pitch_deg = 0.0f;

    // Trip aggregated statistics
    float trip_distance_km = 0.0f;
    float avg_speed_kph = 0.0f;
    float max_speed_kph = 0.0f;
    uint32_t moving_time_sec = 0;

    // System status
    uint8_t battery_percent = 100;
    bool is_recording = true;
};
