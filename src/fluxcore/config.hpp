#ifndef FLUXCORE_CONFIG_HPP
#define FLUXCORE_CONFIG_HPP

#include <cstdint>

namespace fluxcore {

typedef uint8_t byte_t;
typedef byte_t typeid_t;
typedef uint32_t arraySize_t;
typedef uint16_t tupleSize_t;
typedef int64_t int_t;
typedef char32_t char_t;

constexpr std::size_t typedescrlen = 256;

}

#endif

