/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 11.28.2025
/   @author: janpaw
/   @brief: PCA9685 driver (for servo)
*/
#pragma once

#include <Wire.h>
#include <cstdint>
#include <optional>

class PCA9685
{
  private:
    // PCA9685 hardware parameters (from datasheet)
    static constexpr float kOscHz = 25'000'000.0f; // internal oscillator freq
    static constexpr uint16_t kPwmSteps = 4096;    // full PWM cycle resolution
    static constexpr uint16_t kPwmMaxVal = 4095;   // max value for ON/OFF
    static constexpr uint8_t kServoFreqHz = 50; // default servo PWM frequency
    static constexpr uint8_t kNumChannels = 16; // number of PWM channels

    enum class Register : uint8_t
    {
        MODE1 = 0x00,
        MODE2 = 0x01,
        PRESCALE = 0xFE,
        LED0_ON_L = 0x06,
        // no need to define all LED registers as they are sequential
    };

    enum class Mode1Bits : uint8_t
    {
        RESTART = 0b10000000,
        AI = 0b00100000,
        SLEEP = 0b00010000,
    };

    enum class Mode2Bits : uint8_t
    {
        OUTDRV = 0b00000100
    };

  public:
    PCA9685(TwoWire &i2c, uint8_t address_7bit, uint32_t readTimeout = 100, uint32_t wireTimeout = 100);

    [[nodiscard("Check result; PCA9685 init may fail")]]
    bool init();
    [[nodiscard]] bool setPWM(uint8_t channel, uint16_t on, uint16_t off);
    [[nodiscard]] bool setDuty(uint8_t channel, float duty);
    [[nodiscard]] bool setPulseUs(uint8_t channel, float pulseUs);

  private:
    [[nodiscard("Check result; nullopt indicates I2C/device error")]]
    std::optional<uint8_t> readReg(Register reg);
    [[nodiscard]] bool writeReg(Register reg, uint8_t value);
    [[nodiscard]] bool writeMulti(Register reg, uint8_t *data, uint8_t length);

    [[nodiscard]] bool setBit(Register reg, uint8_t bit);
    [[nodiscard]] bool clearBit(Register reg, uint8_t bit);

    [[nodiscard]] bool setFreq();

    TwoWire &m_hi2c;
    uint8_t m_address; // HAL uses 8-bit address
    uint8_t m_freqHz = kServoFreqHz;
    uint32_t m_readTimeout; //read i2c miliseconds timeout
    uint32_t m_wireTimeout; //mileseconds timeout for Wire transmissions
};
