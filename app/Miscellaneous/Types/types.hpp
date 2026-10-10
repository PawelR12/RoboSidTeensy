/**
/   _           __
/  |_) _ |_  _ (_  o  _|
/  | \(_)|_)(_)__) | (_|
/
/   @date: 11.20.2025
/   @author: pawrap
/   @brief: Basic types used to make programs more redable
*/
#pragma once

#include <cstdint>

namespace types
{
    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using u8  = std::uint8_t;
    using u16  = std::uint16_t;
    using u32 = std::uint32_t; 
    using u64 = std::uint64_t;
} // namespace

enum class retTypes : types::u8
{
    OK = 0,
    ERR = 1,
};