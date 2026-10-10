/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 11.29.2025
/   @author: janpaw
/   @brief: Creates JointId enum from jointDefinition.hpp
*/
#pragma once
#include <Robot/jointDefinition.hpp>
#include <cstddef>
#include <cstdint>

namespace robot
{

    enum class JointId : std::uint8_t
    {
    // clang-format off
        #define MAKE_ENUM(name, channel, zeroUs, negLimitUs, posLimitUs) name,

        JOINT_LIST(MAKE_ENUM)

        #undef MAKE_ENUM

        COUNT
        // clang-format on
    };

    constexpr std::size_t jointCount()
    {
        return static_cast<std::size_t>(JointId::COUNT);
    }

    constexpr std::size_t toIndex(JointId id)
    {
        return static_cast<std::size_t>(id);
    }

} // namespace robot
