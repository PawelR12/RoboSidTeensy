/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 11.29.2025
/   @author: janpaw
/   @brief: Servo configuration for each joint from jointDefinition.hpp
*/
#pragma once
#include <Robot/jointDefinition.hpp>
#include <Robot/jointId.hpp>
#include <Types/types.hpp>
#include <array>
#include <cstdint>

using namespace types;

namespace robot
{

    struct ServoConfig
    {
        JointId joint;
        u8 pcaChannel;
        u16 zeroUs;
        u16 negLimitUs;
        u16 posLimitUs;
    };

    constexpr std::array<ServoConfig, jointCount()> ServoConfigs{
    // clang-format off
        #define MAKE_CONFIG(name, channel, zeroUs, negLimitUs, posLimitUs)  \
        ServoConfig{JointId::name, static_cast<std::uint8_t>(channel),      \
                    zeroUs, negLimitUs, posLimitUs},

        JOINT_LIST(MAKE_CONFIG)

        #undef MAKE_CONFIG
        // clang-format on
    };

    static_assert(ServoConfigs.size() == jointCount(),
                  "ServoConfigs size must match JointId::COUNT");

    constexpr const ServoConfig &getServoConfig(JointId id)
    {
        return ServoConfigs[toIndex(id)];
    }

} // namespace robot
