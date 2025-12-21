// murmur_hash3.cpp
//
// MurmurHash3 was written by Austin Appleby, and is placed in the public domain.
// The author hereby disclaims copyright to this source code.
//
// Source: https://github.com/aappleby/smhasher
// Original file: src/MurmurHash3.cpp
//
// Only the x64_128 variant is included here, as that's what we need
// for our Bloom Filter's double-hashing technique.
//
// Minor modifications:
// - Wrapped in quantamental namespace
// - Removed unused x86 variants
// - Added [[fallthrough]] attributes to silence compiler warnings

#include "murmur_hash3.hpp"

namespace quantamental {

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

#if defined(_MSC_VER)
// Microsoft Visual Studio
#define FORCE_INLINE __forceinline
#define ROTL64(x,y) _rotl64(x,y)
#define BIG_CONSTANT(x) (x)

#else
// GCC / Clang
#define FORCE_INLINE inline __attribute__((always_inline))

inline uint64_t rotl64(uint64_t x, int8_t r) {
    return (x << r) | (x >> (64 - r));
}

#define ROTL64(x,y) rotl64(x,y)
#define BIG_CONSTANT(x) (x##LLU)

#endif

//-----------------------------------------------------------------------------
// Block read - handles unaligned reads

FORCE_INLINE uint64_t getblock64(const uint64_t* p, int i) {
    return p[i];
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits to avalanche

FORCE_INLINE uint64_t fmix64(uint64_t k) {
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xff51afd7ed558ccd);
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
    k ^= k >> 33;
    return k;
}

//-----------------------------------------------------------------------------
// MurmurHash3_x64_128

void MurmurHash3_x64_128(const void* key, int len, uint32_t seed, void* out) {
    const uint8_t* data = static_cast<const uint8_t*>(key);
    const int nblocks = len / 16;

    uint64_t h1 = seed;
    uint64_t h2 = seed;

    const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
    const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

    //----------
    // body

    const uint64_t* blocks = reinterpret_cast<const uint64_t*>(data);

    for (int i = 0; i < nblocks; i++) {
        uint64_t k1 = getblock64(blocks, i * 2 + 0);
        uint64_t k2 = getblock64(blocks, i * 2 + 1);

        k1 *= c1;
        k1 = ROTL64(k1, 31);
        k1 *= c2;
        h1 ^= k1;

        h1 = ROTL64(h1, 27);
        h1 += h2;
        h1 = h1 * 5 + 0x52dce729;

        k2 *= c2;
        k2 = ROTL64(k2, 33);
        k2 *= c1;
        h2 ^= k2;

        h2 = ROTL64(h2, 31);
        h2 += h1;
        h2 = h2 * 5 + 0x38495ab5;
    }

    //----------
    // tail

    const uint8_t* tail = data + nblocks * 16;

    uint64_t k1 = 0;
    uint64_t k2 = 0;

    switch (len & 15) {
        case 15: k2 ^= static_cast<uint64_t>(tail[14]) << 48; [[fallthrough]];
        case 14: k2 ^= static_cast<uint64_t>(tail[13]) << 40; [[fallthrough]];
        case 13: k2 ^= static_cast<uint64_t>(tail[12]) << 32; [[fallthrough]];
        case 12: k2 ^= static_cast<uint64_t>(tail[11]) << 24; [[fallthrough]];
        case 11: k2 ^= static_cast<uint64_t>(tail[10]) << 16; [[fallthrough]];
        case 10: k2 ^= static_cast<uint64_t>(tail[9]) << 8;   [[fallthrough]];
        case 9:  k2 ^= static_cast<uint64_t>(tail[8]) << 0;
                 k2 *= c2;
                 k2 = ROTL64(k2, 33);
                 k2 *= c1;
                 h2 ^= k2;
                 [[fallthrough]];

        case 8:  k1 ^= static_cast<uint64_t>(tail[7]) << 56; [[fallthrough]];
        case 7:  k1 ^= static_cast<uint64_t>(tail[6]) << 48; [[fallthrough]];
        case 6:  k1 ^= static_cast<uint64_t>(tail[5]) << 40; [[fallthrough]];
        case 5:  k1 ^= static_cast<uint64_t>(tail[4]) << 32; [[fallthrough]];
        case 4:  k1 ^= static_cast<uint64_t>(tail[3]) << 24; [[fallthrough]];
        case 3:  k1 ^= static_cast<uint64_t>(tail[2]) << 16; [[fallthrough]];
        case 2:  k1 ^= static_cast<uint64_t>(tail[1]) << 8;  [[fallthrough]];
        case 1:  k1 ^= static_cast<uint64_t>(tail[0]) << 0;
                 k1 *= c1;
                 k1 = ROTL64(k1, 31);
                 k1 *= c2;
                 h1 ^= k1;
    }

    //----------
    // finalization

    h1 ^= len;
    h2 ^= len;

    h1 += h2;
    h2 += h1;

    h1 = fmix64(h1);
    h2 = fmix64(h2);

    h1 += h2;
    h2 += h1;

    // Output the two 64-bit hash values
    static_cast<uint64_t*>(out)[0] = h1;
    static_cast<uint64_t*>(out)[1] = h2;
}

} // namespace quantamental