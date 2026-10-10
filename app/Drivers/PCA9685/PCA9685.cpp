/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 11.28.2025
/   @author: janpaw
/   @brief: PCA9685 driver (for servo)
*/

#include "PCA9685.hpp"

PCA9685::PCA9685(TwoWire &i2c, uint8_t address_7bit, uint32_t readTimeout, uint32_t wireTimeout)
    : m_hi2c(i2c), m_address(address_7bit), m_readTimeout(readTimeout), m_wireTimeout(wireTimeout)
{
}

bool PCA9685::writeReg(Register reg, uint8_t value)
{
    // return HAL_I2C_Mem_Write(m_hi2c, m_address, static_cast<uint8_t>(reg),
    //                          I2C_MEMADD_SIZE_8BIT, &value, 1, 50) == HAL_OK;
    m_hi2c.beginTransmission(m_address);
    m_hi2c.write(value);
    m_hi2c.endTransmission();
    return true;
}

// Requires MODE1.AI = 1 (set in init())
bool PCA9685::writeMulti(Register reg, uint8_t *data, uint8_t length)
{
    m_hi2c.beginTransmission(m_address);
    for(int i = 0; i < length; i++)
    {
        m_hi2c.write(*(data + i));
    }
    m_hi2c.endTransmission();
    return true;
}

std::optional<uint8_t> PCA9685::readReg(Register reg)
{
    uint8_t value = 0;
    // sends request to read
    m_hi2c.beginTransmission(m_address);
    m_hi2c.write(static_cast<uint8_t>(reg));
    m_hi2c.endTransmission();
    // read
    m_hi2c.requestFrom(m_address, static_cast<uint8_t>(1));
    uint32_t start = millis();
    while (m_hi2c.available()) 
    {
        value = m_hi2c.read();
        if (millis() - start >= m_readTimeout) 
        {
            return std::nullopt;
        }
    }
    return value;
}

bool PCA9685::init()
{
    m_hi2c.begin();
    m_hi2c.setTimeout(m_wireTimeout);
    // wake from sleep
    if (!clearBit(Register::MODE1, static_cast<uint8_t>(Mode1Bits::SLEEP)))
        return false;

    // set MODE2 push-pull
    if (!writeReg(Register::MODE2, static_cast<uint8_t>(Mode2Bits::OUTDRV)))
        return false;

    // set MODE1 auto-increment
    if (!setBit(Register::MODE1, static_cast<uint8_t>(Mode1Bits::AI)))
        return false;

    // set frequency
    if (!setFreq())
        return false;

    return true;
}

bool PCA9685::setFreq()
{
    float prescale_f = (kOscHz / (static_cast<float>(kPwmSteps) *
                                  static_cast<float>(m_freqHz))) -
                       1.0f;
    uint8_t prescale = static_cast<uint8_t>(prescale_f + 0.5f);

    // go to sleep
    auto oldmode = readReg(Register::MODE1);
    if (!oldmode)
        return false;

    uint8_t sleepmode =
        (*oldmode & 0x7F) | static_cast<uint8_t>(Mode1Bits::SLEEP);
    if (!writeReg(Register::MODE1, sleepmode))
        return false;

    // write prescale
    if (!writeReg(Register::PRESCALE, prescale))
        return false;

    // restore mode
    if (!writeReg(Register::MODE1, *oldmode))
        return false;
    // HAL_Delay(1); TODO: JP, this casued program to hang

    // restart
    if (!writeReg(Register::MODE1,
                  *oldmode | static_cast<uint8_t>(Mode1Bits::RESTART)))
        return false;

    return true;
}

bool PCA9685::setPWM(uint8_t channel, uint16_t on, uint16_t off)
{
    if (channel >= kNumChannels)
        return false;

    uint8_t base = static_cast<uint8_t>(Register::LED0_ON_L) + 4 * channel;

    uint8_t data[4] = {
        static_cast<uint8_t>(on & 0xFF), static_cast<uint8_t>(on >> 8),
        static_cast<uint8_t>(off & 0xFF), static_cast<uint8_t>(off >> 8)};

    return writeMulti(static_cast<Register>(base), data, 4);
}

bool PCA9685::setDuty(uint8_t channel, float duty)
{
    if (channel >= kNumChannels)
        return false;

    if (duty < 0.0f)
        duty = 0.0f;
    if (duty > 1.0f)
        duty = 1.0f;

    uint16_t off =
        static_cast<uint16_t>(static_cast<float>(kPwmMaxVal) * duty + 0.5f);
    return setPWM(channel, 0, off);
}

bool PCA9685::setPulseUs(uint8_t channel, float pulseUs)
{
    if (channel >= kNumChannels)
        return false;

    if (pulseUs < 0.0f)
        pulseUs = 0.0f;

    float periodUs = 1'000'000.0f / static_cast<float>(m_freqHz);

    if (pulseUs > periodUs)
        pulseUs = periodUs;

    float duty = pulseUs / periodUs; // normalize to 0..1

    return setDuty(channel, duty);
}

bool PCA9685::setBit(Register reg, uint8_t bit)
{
    auto v = readReg(reg);
    if (!v)
        return false;
    uint8_t newv = *v | bit;
    return writeReg(reg, newv);
}

bool PCA9685::clearBit(Register reg, uint8_t bit)
{
    auto v = readReg(reg);
    if (!v)
        return false;
    uint8_t newv = *v & ~bit;
    return writeReg(reg, newv);
}
