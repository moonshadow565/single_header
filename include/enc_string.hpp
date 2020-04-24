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
#include <array>
#include <string>
#include <utility>

template<size_t S>
struct EncString {
    char buffer[S + 4] = {};
private:
    template<size_t I>
    static inline constexpr char key() noexcept {
        constexpr char k[] = "abcdefghjklmnop";
        return k[I % (sizeof(k) - 1)];
    }
    template<size_t...I>
    inline constexpr EncString(char const(&str)[S], std::index_sequence<I...>)
        noexcept : buffer {(char)((str[I] ^ key<I>()) & 0xFF)...} {}
    template<size_t...I>
    inline std::array<char, S> decrypt(std::index_sequence<I...>) const noexcept {
        auto str = (char const volatile*)buffer;
        return {(char)((str[I] ^ key<I>()) & 0xFF)...};
    }
public:
    inline constexpr EncString(char const (&str)[S]) noexcept : EncString(str, std::make_index_sequence<S>()) {}
    inline constexpr EncString(EncString const&) noexcept = default;
    inline constexpr EncString(EncString&&) noexcept = default;
    inline constexpr EncString& operator=(EncString const&) noexcept = default;
    inline constexpr EncString& operator=(EncString&&) noexcept = default;
    inline std::array<char, S> array() const noexcept { return decrypt(std::make_index_sequence<S>()); };
    inline std::string str() const noexcept { return array().data(); }
};
