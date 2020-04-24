/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
✂--------------------------------[ Cut here ]----------------------------------
*/
#pragma once
#include <cstddef>
#include <cinttypes>

constexpr uint32_t XXH32(char const* data, size_t size, uint32_t seed = 0) noexcept {
    constexpr uint32_t Prime1 = 2654435761U;
    constexpr uint32_t Prime2 = 2246822519U;
    constexpr uint32_t Prime3 = 3266489917U;
    constexpr uint32_t Prime4 =  668265263U;
    constexpr uint32_t Prime5 =  374761393U;

    constexpr auto Char = [](char c) constexpr -> uint32_t {
        // return static_cast<uint8_t>(c >= 'A' && c <= 'Z' ? c + (- 'A' + 'a') : c);
        return static_cast<uint8_t>(c);
    };
    constexpr auto Block = [Char](char const* data) constexpr -> uint32_t {
        return Char(*data)
                | (Char(*(data + 1)) << 8)
                | (Char(*(data + 2)) << 16)
                | (Char(*(data + 3)) << 24);
    };
    constexpr auto ROL = [](uint32_t x, unsigned char bits) constexpr -> uint32_t {
        return (x << bits) | (x >> (32 - bits));
    };

    char const* const end = data + size;
    uint32_t result = static_cast<uint32_t>(size);
    if(result >= 16) {
        uint32_t state[4] = {
            seed + Prime1 + Prime2,
            seed + Prime2,
            seed,
            seed - Prime1
        };
        for(; (data + 16) <= end; data += 16) {
            state[0] = ROL(state[0] + Block(data) * Prime2, 13) * Prime1;
            state[1] = ROL(state[1] + Block(data + 4) * Prime2, 13) * Prime1;
            state[2] = ROL(state[2] + Block(data + 8) * Prime2, 13) * Prime1;
            state[3] = ROL(state[3] + Block(data + 12) * Prime2, 13) * Prime1;
        }
        result += ROL(state[0],  1) +
                  ROL(state[1],  7) +
                  ROL(state[2], 12) +
                  ROL(state[3], 18);
    } else {
        result += seed + Prime5;
    }
    for(; data + 4 <= end; data += 4) {
        result = ROL(result + Block(data) * Prime3, 17) * Prime4;
    }
    for(; data != end; data++) {
        result = ROL(result + Char(*data) * Prime5, 11) * Prime1;
    }

    result ^= result >> 15;
    result *= Prime2;
    result ^= result >> 13;
    result *= Prime3;
    result ^= result >> 16;
    return result;
}