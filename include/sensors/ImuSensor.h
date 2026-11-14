#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "sensors/ISensor.h"


enum class ImuType
{
    NONE = 0,
    MPU6050,
    QMI8658
};


class ImuSensor : public ISensor
{
public:
    ImuSensor();

    const char* getName() const override { return "IMU"; }
    bool begin() override;
    void update() override;
    bool isConnected() const override { return (detected_type != ImuType::NONE); }

    // Motion and Orientation metrics
    float getPitchDeg() const { return pitch_deg; }
    float getRollDeg() const { return roll_deg; }
    float getGradePercent() const { return grade_percent; }
    float getAccelMagnitude() const { return accel_mag; }
    bool isMoving() const { return moving; }

    ImuType getType() const { return detected_type; }
    const char* getTypeName() const;

private:
    ImuType detected_type = ImuType::NONE;
    uint8_t i2c_addr = 0x00;

    // Raw acceleration in Gs
    float ax = 0.0f;
    float ay = 0.0f;
    float az = 1.0f;

    // Computed angles
    float pitch_deg = 0.0f;
    float roll_deg = 0.0f;
    float grade_percent = 0.0f;
    float accel_mag = 1.0f;
    bool moving = false;

    bool initMpu6050();
    bool initQmi8658();
    void readMpu6050();
    void readQmi8658();
};
