#pragma once

#include <cstdint>

struct MotorControlMessage
{
    uint8_t servoNum;
    uint16_t val;
};
