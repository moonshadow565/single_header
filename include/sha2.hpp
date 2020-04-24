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

namespace sha2 {
    template<bool IS512 = false>
    struct SHA2_CTX final {
    private:
        using uint_t = std::conditional_t<IS512, uint64_t, uint32_t>;
        std::array<uint_t, 8> state_ = {};
        uint8_t buffer_[IS512 ? 128 : 64] = {};
        uint64_t buffer_total_ = {};
        size_t buffer_size_ = {};

        static inline constexpr auto K = []{
            if constexpr (IS512) {
                return std::array<uint64_t, 80> {
                    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL,
                    0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
                    0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL,
                    0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
                    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
                    0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
                    0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL, 0x2de92c6f592b0275ULL,
                    0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
                    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL,
                    0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
                    0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL,
                    0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
                    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL,
                    0x92722c851482353bULL, 0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
                    0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
                    0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
                    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL,
                    0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
                    0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL,
                    0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
                    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL,
                    0xc67178f2e372532bULL, 0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
                    0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL,
                    0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
                    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
                    0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
                    0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL,
                };
            } else {
                return std::array<uint32_t, 64> {
                    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
                    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
                    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
                    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
                    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
                    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
                    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
                    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
                    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
                };
            }
        }();

        template<typename T>
        static inline constexpr void READ(uint8_t const* data, T& value) noexcept {
            T tmp = 0;
            size_t index = 0;
            do {
                tmp <<= 8;
                tmp |= data[index];
                index++;
            } while(index != sizeof(T));
            value = tmp;
        }

        template<typename T>
        static inline constexpr void WRITE(uint8_t* data, T const& value) noexcept {
            T tmp = value;
            size_t index = sizeof(T);
            do {
                index--;
                data[index] = tmp & 0xFF;
                tmp >>= 8;
            } while(index);
        }

        static inline constexpr uint_t CH(uint_t x, uint_t y, uint_t z) noexcept {
            return (x & y) ^ (~x & z);
        }

        static inline constexpr uint_t MAJ(uint_t x, uint_t y, uint_t z) noexcept {
            return (x & y) ^ (x & z) ^ (y & z);
        }

        template<size_t b>
        static inline constexpr uint_t ROR(uint_t a) noexcept {
            return (a >> b) | (a << (sizeof(uint_t) * 8 - b));
        }

        static inline constexpr uint_t EP0(uint_t x) noexcept {
            if constexpr (IS512) {
                return ROR<28>(x) ^ ROR<34>(x) ^ ROR<39>(x);
            } else {
                return ROR<2>(x) ^ ROR<13>(x) ^ ROR<22>(x);
            }
        }

        static inline constexpr uint_t EP1(uint_t x) noexcept {
            if constexpr (IS512) {
                return ROR<14>(x) ^ ROR<18>(x) ^ ROR<41>(x);
            } else {
                return ROR<6>(x) ^ ROR<11>(x) ^ ROR<25>(x);
            }
        }

        static inline constexpr uint_t SIG0(uint_t x) noexcept {
            if constexpr (IS512) {
                return ROR<1>(x) ^ ROR<8>(x) ^ (x >> 7);
            } else {
                return ROR<7>(x) ^ ROR<18>(x) ^ (x >> 3);
            }
        }

        static inline constexpr uint_t SIG1(uint_t x) noexcept {
            if constexpr (IS512) {
                return ROR<19>(x) ^ ROR<61>(x) ^ (x >> 6);
            } else {
                return ROR<17>(x) ^ ROR<19>(x) ^ (x >> 10);
            }
        }

        constexpr void transform() noexcept {
            uint_t m[K.size()] = {};
            for (size_t i = 0; i != 16; ++i) {
                READ(buffer_ + i * sizeof(uint_t), m[i]);
            }
            for (size_t i = 16 ; i != K.size(); ++i) {
                m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];
            }
            auto s = state_;
            for (size_t i = 0; i != K.size(); ++i) {
                auto t1 = s[7] + EP1(s[4]) + CH(s[4], s[5], s[6]) + K[i] + m[i];
                auto t2 = EP0(s[0]) + MAJ(s[0], s[1], s[2]);
                s = { t1 + t2, s[0], s[1], s[2], s[3] + t1, s[4], s[5], s[6] };
            }
            for (size_t i = 0; i != state_.size(); ++i) {
                state_[i] += s[i];
            }
        }
    public:
        constexpr void init() noexcept {
            if constexpr (IS512) {
                state_ = {
                    0x6a09e667f3bcc908ull, 0xbb67ae8584caa73bull, 0x3c6ef372fe94f82bull,
                    0xa54ff53a5f1d36f1ull, 0x510e527fade682d1ull, 0x9b05688c2b3e6c1full,
                    0x1f83d9abfb41bd6bull, 0x5be0cd19137e2179ull,
                };
            } else {
                state_ = {
                    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab,
                    0x5be0cd19,
                };
            }
            buffer_total_ = {};
            buffer_size_ = {};
        }

        template<typename Char>
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
            if constexpr(IS512){
                uint8_t bitssize[16] = {};
                WRITE(bitssize + 8, buffer_total_ * 8);
                update(pad, 1 + ((239 - buffer_size_) % 128));
                update(bitssize, 16);
            } else {
                uint8_t bitssize[8] = {};
                WRITE(bitssize, buffer_total_ * 8);
                update(pad, 1 + ((119 - buffer_size_) % 64));
                update(bitssize, 8);
            }
        }

        constexpr void digest(uint8_t hash[IS512 ? 64 : 32]) const noexcept {
            for (size_t i = 0; i != 8; i++) {
                WRITE(hash + i * sizeof(uint_t), state_[i]);
            }
        }
    };

    using SHA256_CTX = SHA2_CTX<false>;

    inline constexpr int SHA256_Init(SHA256_CTX* c) noexcept {
        c->init();
        return 1;
    }

    template<typename Char>
    inline constexpr int SHA256_Update(SHA256_CTX *c, Char const* data, size_t len) noexcept {
        c->update(data, len);
        return 1;
    }

    inline constexpr int SHA256_Final(uint8_t md[32], SHA256_CTX *c) noexcept {
        c->finish();
        c->digest(md);
        return 1;
    }

    template<typename Char>
    inline constexpr uint8_t* SHA256(Char const* d, size_t n, uint8_t md[32]) noexcept {
        auto ctx = SHA256_CTX{};
        ctx.init();
        ctx.update(d, n);
        ctx.finish();
        ctx.digest(md);
        return md;
    }

    using SHA512_CTX = SHA2_CTX<true>;

    inline constexpr int SHA512_Init(SHA512_CTX* c) noexcept {
        c->init();
        return 1;
    }

    template<typename Char>
    inline constexpr int SHA512_Update(SHA512_CTX *c, Char const* data, size_t len) noexcept {
        c->update(data, len);
        return 1;
    }

    inline constexpr int SHA512_Final(uint8_t md[64], SHA512_CTX *c) noexcept {
        c->finish();
        c->digest(md);
        return 1;
    }

    template<typename Char>
    inline constexpr uint8_t* SHA512(Char const* d, size_t n, uint8_t md[64]) noexcept {
        auto ctx = SHA512_CTX{};
        ctx.init();
        ctx.update(d, n);
        ctx.finish();
        ctx.digest(md);
        return md;
    }
}
