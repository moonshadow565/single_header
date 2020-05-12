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
#include <array>
#include <algorithm>

template<size_t N>
inline constexpr auto make_pattern(char const(&str)[N]) noexcept {
    constexpr auto table = []() {
        std::array<uint8_t, 256> result = {};
        for (uint8_t i = 0; i != 10; i++) {
            result['0' + i] = i;
        }
        for (uint8_t i = 0; i != 6; i++) {
            result['a' + i] = 10 + i;
            result['A' + i] = 10 + i;
        }
        result['?'] = 0xF0u;
        result['x'] = 0xF0u;
        return result;
    }();
    std::array<uint16_t, N / 3> ops = {};
    auto s = str;
    for (auto& o: ops) {
        uint16_t const hi = table[*s++];
        uint16_t const lo = table[*s++];
        s++;
        o = (hi << 4) | lo;
    }
    return [ops](char const* begin, size_t size) noexcept -> char const* {
        auto const result = std::search(begin, begin + size,
                           ops.begin(), ops.end(), [](char left, uint16_t right) noexcept {
            return right > 0xFF || (uint8_t)left == right;
        });
        return result == begin + size ? nullptr : result;
    };
}
