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
#include <bit>

namespace md5 {
    struct MD5_CTX {
    private:
        static constexpr std::array<uint32_t, 64> K = {
            0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
            0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
            0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
            0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
            0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
            0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
            0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
            0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
        };

        static constexpr std::array<uint8_t, 64> S = {
            7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,  //
            5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,  //
            4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,  //
            6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,  //
        };

        static constexpr std::array<uint8_t, 64> I = {
            0, 1, 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,  //
            1, 6, 11, 0,  5,  10, 15, 4,  9,  14, 3,  8,  13, 2,  7,  12,  //
            5, 8, 11, 14, 1,  4,  7,  10, 13, 0,  3,  6,  9,  12, 15, 2,   //
            0, 7, 14, 5,  12, 3,  10, 1,  8,  15, 6,  13, 4,  11, 2,  9,
        };

        static constexpr std::array<uint32_t, 4> T = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};

        alignas(16) std::array<uint32_t, 4> state_ = {};
        alignas(16) std::array<uint8_t, 64> buffer_{};
        uint64_t buffer_total_ = {};
        size_t buffer_size_ = {};

        template <typename T, size_t S, size_t R = S / sizeof(T)>
        static constexpr std::array<T, R> from_bytes(std::array<uint8_t, S> const& data) noexcept {
            static_assert(S % sizeof(T) == 0);
            if constexpr (sizeof(T) == sizeof(uint8_t) || std::endian::native == std::endian::little) {
                return std::bit_cast<std::array<T, R>>(data);
            } else if constexpr (std::endian::native == std::endian::big) {
                auto tmp = std::bit_cast<std::array<T, R>>(data);
                for (auto& t : tmp) {
                    t = std::byteswap(t);
                }
                return tmp;
            } else {
                auto result = std::array<T, R>{};
                for (size_t d = 0, r = 0; d != S; ++r, d += sizeof(T)) {
                    for (size_t i = 0; i != sizeof(T); ++i) {
                        result[r] |= ((T)data[d + i]) << (8 * i);
                    }
                }
                return result;
            }
        }

        template <typename T, size_t S, size_t R = S * sizeof(T)>
        static constexpr std::array<uint8_t, R> to_bytes(std::array<T, S> const& data) noexcept {
            if constexpr (sizeof(T) == sizeof(uint8_t) || std::endian::native == std::endian::little) {
                return std::bit_cast<std::array<uint8_t, R>>(data);
            } else if constexpr (std::endian::native == std::endian::big) {
                auto tmp = data;
                for (auto& t : tmp) {
                    t = std::byteswap(t);
                }
                return std::bit_cast<std::array<uint8_t, R>>(tmp);
            } else {
                auto result = std::array<uint8_t, R>{};
                for (size_t d = 0, r = 0; d != S; ++d, r += sizeof(T)) {
                    for (size_t i = 0; i != sizeof(T); ++i) {
                        result[r + i] = (uint8_t)(data[d] >> (i * 8));
                    }
                }
                return result;
            }
        }

        constexpr void transform() noexcept {
            auto [A, B, C, D] = state_;
            auto const blocks = from_bytes<uint32_t>(buffer_);

            for (size_t i = 0; i != 64; ++i) {
                auto acc = A + K[i] + blocks[I[i]];
                acc += (i <= 15)   ? ((B & C) | (~B & D))
                       : (i <= 31) ? ((D & B) | (~D & C))
                       : (i <= 47) ? (B ^ C ^ D)
                                   : (C ^ (B | ~D));
                A = D;
                D = C;
                C = B;
                B += std::rotl(acc, S[i]);
            }

            state_[0] += A;
            state_[1] += B;
            state_[2] += C;
            state_[3] += D;
        }

    public:
        constexpr MD5_CTX() noexcept = default;

        constexpr void init() noexcept {
            state_ = T;
            buffer_ = {};
            buffer_total_ = {};
            buffer_size_ = {};
        }

        template <typename Char>
        constexpr void update(Char const* data, size_t size) noexcept {
            static_assert(sizeof(Char) == 1);
            buffer_total_ += size;
            if (buffer_size_ && buffer_size_ + size >= sizeof(buffer_)) {
                size_t diff = sizeof(buffer_) - buffer_size_;
                for (size_t i = 0; i != diff; i++) {
                    buffer_[buffer_size_++] = (uint8_t)data[i];
                }
                data += diff;
                size -= diff;
                transform();
                buffer_size_ = 0;
            }
            while (size >= sizeof(buffer_)) {
                for (size_t i = 0; i != sizeof(buffer_); i++) {
                    buffer_[i] = (uint8_t)data[i];
                }
                data += sizeof(buffer_);
                size -= sizeof(buffer_);
                transform();
            }
            if (size) {
                for (size_t i = 0; i != size; i++) {
                    buffer_[buffer_size_] = (uint8_t)data[i];
                    buffer_size_++;
                    if (buffer_size_ == sizeof(buffer_)) {
                        buffer_size_ = 0;
                    }
                }
            }
        }

        constexpr void finish() noexcept {
            uint8_t pad[sizeof(buffer_)] = {0x80};
            auto const bitssize = to_bytes(std::array{buffer_total_ * 8});
            update(pad, 1 + ((119 - buffer_size_) % 64));
            update(bitssize.data(), bitssize.size());
        }

        constexpr void digest(uint8_t hash[16]) const noexcept {
            auto const result = to_bytes(state_);
            for (size_t i = 0; i != 16; ++i) {
                hash[i] = result[i];
            }
        }
    };

    constexpr int MD5_Init(MD5_CTX* c) noexcept {
        c->init();
        return 1;
    }

    template <typename Char>
    constexpr int MD5_Update(MD5_CTX* c, Char const* data, size_t len) noexcept {
        c->update(data, len);
        return 1;
    }

    constexpr int MD5_Final(uint8_t md[16], MD5_CTX* c) noexcept {
        c->finish();
        c->digest(md);
        return 1;
    }

    template <typename Char>
    constexpr uint8_t* MD5(Char const* d, size_t n, uint8_t md[16]) noexcept {
        auto ctx = MD5_CTX{};
        ctx.init();
        ctx.update(d, n);
        ctx.finish();
        ctx.digest(md);
        return md;
    }
}
