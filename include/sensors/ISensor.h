#pragma once


/**
 * @brief Abstract sensor interface for all bike computer data sources.
 */
class ISensor
{
public:
    virtual ~ISensor() = default;

    virtual const char* getName() const = 0;
    virtual bool begin() = 0;
    virtual void update() = 0;
    virtual bool isConnected() const = 0;
};
