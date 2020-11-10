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
#include <cstring>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

namespace ppp {
    template <char TAG, std::uint16_t START, std::uint16_t SIZE>
    struct ct_group {
        static constexpr inline char tag = TAG;
        static constexpr inline std::uint16_t start = START;
        static constexpr inline std::uint16_t size = SIZE;
    };

    template <std::uint16_t OP>
    struct ct_op {
        static constexpr inline std::uint16_t op = OP;
    };

    template <typename...ELEMS>
    struct ct_array;

    template<>
    struct ct_array<> {
        static constexpr std::uint16_t size = 0;

        template <std::int32_t I>
        using get = void;
    };

    template <typename FIRST, typename...REST>
    struct ct_array<FIRST, REST...> : ct_array <REST...> {
        static constexpr std::uint16_t size = 1 + sizeof...(REST);

        template <std::int32_t I>
        using get = std::conditional_t<I == 0, FIRST, typename ct_array <REST...>::template get<I - 1>>;
    };

    template <typename OPS, typename GROUPS>
    struct ct_pattern {
        using ops = OPS;
        using groups = GROUPS;
    };

    struct group {
        char tag = {};
        std::uint16_t start = {};
        std::uint16_t size = {};
    };

    template <size_t N>
    struct pattern {
        std::uint16_t op_count = {};
        std::uint16_t ops[N] = {};
        std::uint16_t group_count = {};
        group groups[N] = {};
    private:
        static constexpr int32_t parse_hex(char c) noexcept  {
            if (c >= '0' && c <= '9') {
                return static_cast<uint8_t>(c - '0');
            } else if (c >= 'a' && c <= 'f') {
                return static_cast<uint8_t>(c - 'a' + 10);
            } else if (c >= 'A' && c <= 'F') {
                return static_cast<uint8_t>(c - 'A' + 10);
            } else {
                return -1;
            }
        };

        constexpr void parse_cap_group(char const*& next) {
            std::uint16_t group_index = group_count++;
            std::uint16_t group_op_start = op_count;
            for (;;) {
                if (char cur = *next++; cur == '\0') {
                    if (group_index != 0) {
                        throw "Unterminated capture group!";
                    }
                    break;
                } else if (cur == ']') {
                    if (group_index == 0) {
                        throw "Stray capture group terminator!";
                    }
                    break;
                } else if (cur == '[') {
                    parse_cap_group(next);
                } else if (cur == 's' || cur == 'u' || cur == 'i' || cur == 'o' || cur == 'r') {
                    if (*next != '[') {
                        throw "Capture group tag must be followed by capture group!";
                    }
                    groups[group_count].tag = cur;
                } else if (cur == '.') {
                    ops[op_count++] = 0x100u;
                } else if (cur == '?') {
                    ops[op_count++] = 0x100u;
                    if (*next == '?') {
                        ++next;
                    }
                } else if (auto const first = parse_hex(cur); first != -1) {
                    if (auto const second = parse_hex(*next); second != -1) {
                        ops[op_count++] = static_cast<std::uint16_t>((first << 4) | second);
                        ++next;
                    } else {
                        ops[op_count++] = static_cast<std::uint16_t>(first);
                    }
                } else if (cur == '`') {
                    if (char escaped = *next++; escaped == '\0') {
                        throw "Character literal esacpe at end of string";
                    } else {
                        ops[op_count++] = static_cast<uint8_t>(escaped);
                    }
                } else if (cur == ' ') {
                } else {
                    throw "Invalid pattern character!";
                }
            }
            std::uint16_t group_op_count = op_count - group_op_start;
            if (group_op_count == 0) {
                throw "Empty capture group!";
            }
            groups[group_index].start = group_op_start;
            groups[group_index].size = group_op_count;
        }
    public:
        constexpr pattern(char const (&str)[N]) {
            char const* iter = str;
            parse_cap_group(iter);
        }
    };

    template <typename GROUPS>
    struct result {
    private:
        char const* start_;
        char const* match_;
        char const* end_;
    public:
        constexpr result(char const* start, char const* match, char const* end) noexcept
            : start_(start), match_(match), end_(end) {}

        // Get std::span of source data
        constexpr auto source() const noexcept {
            return std::span { start_, end_ };
        }
        // Get std::span to scan next
        constexpr auto next() const noexcept {
            return std::span { match_ + GROUPS::template get<0>::size , end_ };
        }
        // Get char const* of capture group
        template<std::uint8_t I>
        constexpr auto group_data() const noexcept {
            static_assert (I < GROUPS::size, "Invalid capture group!");
            return match_ + GROUPS::template get<I>::start;
        }
        // Get sizeof of caputre group
        template<std::uint8_t I>
        static consteval auto group_size() noexcept {
            static_assert (I < GROUPS::size, "Invalid capture group!");
            return GROUPS::template get<I>::size;
        }
        // Get std::span of capture group
        template<std::uint8_t I>
        constexpr auto group_span() const noexcept {
            static_assert (I < GROUPS::size, "Invalid capture group!");
            return std::span<char const> { match_, end_ }
                    .subspan<GROUPS::template get<I>::start, GROUPS::template get<I>::size>();
        }
        // Get std::string_view from capture group
        template<std::uint8_t I>
        constexpr auto group_sview() const noexcept {
            static_assert (I < GROUPS::size, "Invalid capture group!");
            return std::string_view { group_data<I>(), group_size<I>() };
        }
        // Reinterpret capture group as unsigned integer
        template<std::uint8_t I>
        constexpr auto group_uint() const noexcept {
            static_assert (I < GROUPS::size, "Invalid capture group!");
            constexpr auto SIZE = group_size<I>();
            static_assert (SIZE == 1 || SIZE == 2 || SIZE == 4 || SIZE == 8, "Invalid integer size!");
            using result_t = std::conditional_t<SIZE == 1, std::uint8_t,
                                std::conditional_t<SIZE == 2, std::uint16_t,
                                    std::conditional_t<SIZE == 4, std::uint32_t,
                                        std::conditional_t<SIZE == 8, std::uint64_t,
                                            void>>>>;
            if (std::is_constant_evaluated()) {
                auto const group = group_data<I>();
                result_t result = (result_t)(std::uint8_t)group[0];
                for (std::size_t i = 1; i != SIZE; ++i) {
                    result |= (result_t)(std::uint8_t)group[i] << (i * 8);
                }
                return result;
            } else {
                result_t result;
                std::memcpy(&result, group_data<I>(), SIZE);
                return result;
            }
        }
        // Reinterpret capture group as integer
        template<std::uint8_t I>
        constexpr auto group_int() const noexcept {
            static_assert (I < GROUPS::size, "Invalid capture group!");
            auto const result = group_uint<I>();
            using T = std::make_signed_t<std::remove_cvref_t<decltype(result)>>;
            return (T)result;
        }
        // Get offset of capture group from start of source data
        template<std::uint8_t I>
        constexpr auto group_off() const noexcept {
            static_assert (I < GROUPS::size, "Invalid capture group!");
            return group_data<I>() - start_;
        }
        // Reinterpet capture group as relative offset from end of itself to start of source of data
        template<std::uint8_t I>
        constexpr auto group_rel() const noexcept {
            static_assert (I < GROUPS::size, "Invalid capture group!");
            auto const offset = group_off<I>();
            auto const relative = group_int<I>();
            return offset + group_size<I>() + relative;
        }
        // Get capture group as hinted: (s)view, (u)int, (i)nt, (o)ff, (r)el else as span
        template<std::uint8_t I>
        constexpr auto group() const noexcept -> decltype(auto) {
            static_assert (I < GROUPS::size, "Invalid capture group!");
            constexpr char tag = GROUPS::template get<I>::tag;
            if constexpr (tag == 's') {
                return group_sview<I>();
            } else if constexpr (tag == 'u') {
                return group_uint<I>();
            } else if constexpr (tag == 'i') {
                return group_int<I>();
            } else if constexpr (tag == 'o') {
                return group_off<I>();
            } else if constexpr (tag == 'r') {
                return group_rel<I>();
            } else {
                return group_span<I>();
            }
        }
    };
}


#if defined(__clang__)
template<class Char = char, Char...c>
consteval auto operator"" _pattern() {
    constexpr auto pat = ppp::pattern<sizeof...(c) + 1>({ c..., '\0' });
#else
template<ppp::pattern pat>
consteval auto operator"" _pattern() noexcept {
#endif
    return []<std::size_t...O, std::size_t...G> (std::index_sequence<O...>, std::index_sequence<G...>) {
        using groups_t = ppp::ct_array<ppp::ct_group<pat.groups[G].tag, pat.groups[G].start, pat.groups[G].size>...>;
        using result_t = ppp::result<groups_t>;
        return +[] (std::span<char const> data) constexpr noexcept -> std::optional<result_t> {
            for (auto i = data; i.size() >= pat.op_count; i = i.subspan(1)) {
                if (((pat.ops[O] > 0xFF ? true : pat.ops[O] == (std::uint8_t)i[O]) && ...)) {
                    return result_t(data.data(), i.data(), data.data() + data.size());
                }
            }
            return std::nullopt;
        };
    } (std::make_index_sequence<pat.op_count>(), std::make_index_sequence<pat.group_count>());
}

