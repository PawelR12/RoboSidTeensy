/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 11.29.2025
/   @author: janpaw
/   @brief: Single source of truth for all physical configurations of joints
*/
#pragma once
#include <Robot/jointCalib.hpp>
#include <Types/types.hpp>

using namespace types;

// ranges - common for each robot
constexpr u16 WRIST_RANGE_US = 1400;
constexpr u16 ARM_RANGE_US = 700;

// TODO: JP, need to callib shoulders after full assembly

// Note: negLimitUs / posLimitUs do NOT describe numeric min/max pulse values.
// They define the pulse width that places the joint at its physical
// negative/positive extreme (u = -1 / u = +1).
// Due to joint mirroring, these values may be either lower or higher than
// zeroUs.

// clang-format off
//  X(name,         ch, zeroUs,             negLimitUs,         posLimitUs                          )
#define JOINT_LIST(X)                                                                                   \
    X(FL_SHOULDER,  0,  1500,               1350,               1600                                )   \
    X(FL_ARM,       1,  FL_ARM_ZERO_US,     820,     2250       )   \
    X(FL_WRIST,     2,  FL_WRIST_ZERO_US,   820,   2250   )   \
                                                                                                        \
    X(FR_SHOULDER,  3, 1500,               1350,               1600                                )   \
    X(FR_ARM,       4, FR_ARM_ZERO_US,     820,     2250       )   \
    X(FR_WRIST,     5,  FR_WRIST_ZERO_US,   820,   2250   )   \
                                                                                                        \
    X(RL_SHOULDER,  6,  1500,               1300,               1600                                )   \
    X(RL_ARM,       7,  RL_ARM_ZERO_US,     820,     2250        )   \
    X(RL_WRIST,     8,  RL_WRIST_ZERO_US,   820,   2250    )   \
                                                                                                        \
    X(RR_SHOULDER,  9,  1500,               1250,               1600                                )   \
    X(RR_ARM,      10,  RR_ARM_ZERO_US,     820,     2250        )   \
    X(RR_WRIST,    11,  RR_WRIST_ZERO_US,   820,   2250 )

// clang-format on