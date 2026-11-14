#include "sensors/ImuSensor.h"
#include <cmath>


ImuSensor::ImuSensor()
{
}


const char* ImuSensor::getTypeName() const
{
    switch (detected_type)
    {
        case ImuType::MPU6050: return "MPU6050";
        case ImuType::QMI8658: return "QMI8658";
        default:              return "None";
    }
}


bool ImuSensor::begin()
{
    detected_type = ImuType::NONE;

    // 1. Try detecting MPU6050 at 0x68
    Wire.beginTransmission(0x68);
    Wire.write(0x75); // WHO_AM_I register
    if (Wire.endTransmission(false) == 0)
    {
        if (Wire.requestFrom((uint8_t)0x68, (uint8_t)1) == 1)
        {
            uint8_t who = Wire.read();
            if (who == 0x68 || who == 0x70 || who == 0x71 || who == 0x72)
            {
                i2c_addr = 0x68;
                if (initMpu6050())
                {
                    detected_type = ImuType::MPU6050;
                    return true;
                }
            }
        }
    }

    // 2. Try detecting QMI8658 at 0x6B
    Wire.beginTransmission(0x6B);
    Wire.write(0x00); // WHO_AM_I register
    if (Wire.endTransmission(false) == 0)
    {
        if (Wire.requestFrom((uint8_t)0x6B, (uint8_t)1) == 1)
        {
            uint8_t who = Wire.read();
            if (who == 0x05)
            {
                i2c_addr = 0x6B;
                if (initQmi8658())
                {
                    detected_type = ImuType::QMI8658;
                    return true;
                }
            }
        }
    }

    return false;
}


bool ImuSensor::initMpu6050()
{
    // Wake up MPU6050 by clearing sleep bit in PWR_MGMT_1 (0x6B)
    Wire.beginTransmission(i2c_addr);
    Wire.write(0x6B);
    Wire.write(0x00);
    if (Wire.endTransmission() != 0) return false;

    // Set Accelerometer to +/- 4G (0x1C = 0x08)
    Wire.beginTransmission(i2c_addr);
    Wire.write(0x1C);
    Wire.write(0x08);
    return (Wire.endTransmission() == 0);
}


bool ImuSensor::initQmi8658()
{
    // CTRL1 (0x02): Enable address auto-increment
    Wire.beginTransmission(i2c_addr);
    Wire.write(0x02);
    Wire.write(0x60);
    Wire.endTransmission();

    // CTRL2 (0x03): Accelerometer +/- 4G, 100Hz ODR
    Wire.beginTransmission(i2c_addr);
    Wire.write(0x03);
    Wire.write(0x23);
    Wire.endTransmission();

    // CTRL7 (0x08): Enable accelerometer and gyroscope
    Wire.beginTransmission(i2c_addr);
    Wire.write(0x08);
    Wire.write(0x03);
    return (Wire.endTransmission() == 0);
}


void ImuSensor::update()
{
    if (detected_type == ImuType::MPU6050)
    {
        readMpu6050();
    }
    else if (detected_type == ImuType::QMI8658)
    {
        readQmi8658();
    }
    else
    {
        return;
    }

    // Compute total acceleration magnitude
    accel_mag = sqrtf(ax * ax + ay * ay + az * az);

    // Compute Pitch (road gradient / incline angle in degrees)
    pitch_deg = atan2f(-ax, sqrtf(ay * ay + az * az)) * (180.0f / M_PI);
    roll_deg  = atan2f(ay, az) * (180.0f / M_PI);

    // Grade percentage = tan(pitch) * 100%
    grade_percent = tanf(pitch_deg * (M_PI / 180.0f)) * 100.0f;

    // Motion detection: deviation from 1.0G gravity
    moving = (fabsf(accel_mag - 1.0f) > 0.12f);
}


void ImuSensor::readMpu6050()
{
    Wire.beginTransmission(i2c_addr);
    Wire.write(0x3B); // ACCEL_XOUT_H
    if (Wire.endTransmission(false) != 0) return;

    if (Wire.requestFrom((uint8_t)i2c_addr, (uint8_t)6) == 6)
    {
        int16_t raw_x = (Wire.read() << 8) | Wire.read();
        int16_t raw_y = (Wire.read() << 8) | Wire.read();
        int16_t raw_z = (Wire.read() << 8) | Wire.read();

        // +/- 4G scale = 8192 LSB/g
        ax = (float)raw_x / 8192.0f;
        ay = (float)raw_y / 8192.0f;
        az = (float)raw_z / 8192.0f;
    }
}


void ImuSensor::readQmi8658()
{
    Wire.beginTransmission(i2c_addr);
    Wire.write(0x35); // QMI8658 Ax_L register
    if (Wire.endTransmission(false) != 0) return;

    if (Wire.requestFrom((uint8_t)i2c_addr, (uint8_t)6) == 6)
    {
        int16_t raw_x = Wire.read() | (Wire.read() << 8);
        int16_t raw_y = Wire.read() | (Wire.read() << 8);
        int16_t raw_z = Wire.read() | (Wire.read() << 8);

        // +/- 4G scale = 8192 LSB/g
        ax = (float)raw_x / 8192.0f;
        ay = (float)raw_y / 8192.0f;
        az = (float)raw_z / 8192.0f;
    }
}
