/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 11.29.2025
/   @author: janpaw
/   @brief: Robot unit specific calibration
*/
#pragma once
#include <Types/types.hpp>

using namespace types;

// these values will be unique for each Robot unit
// TODO: JP, pushing it now but later we may want to add it to gitignore or have
// per-unit config files

// wrists
constexpr u16 FL_WRIST_ZERO_US = 1500;
constexpr u16 FR_WRIST_ZERO_US = 1500;
constexpr u16 RL_WRIST_ZERO_US = 1500;
constexpr u16 RR_WRIST_ZERO_US = 1500;

// arms
constexpr u16 FL_ARM_ZERO_US = 1500;
constexpr u16 FR_ARM_ZERO_US = 1500;
constexpr u16 RL_ARM_ZERO_US = 1500;
constexpr u16 RR_ARM_ZERO_US = 1500;

// TODO: JP, need to callib shoulders after full assembly
