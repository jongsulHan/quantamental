// murmur_hash3.hpp
//
// MurmurHash3 was written by Austin Appleby, and is placed in the public domain.
// The author hereby disclaims copyright to this source code.
//
// Source: https://github.com/aappleby/smhasher
//
// This is the x64 128-bit variant, which produces two 64-bit hash values.
// We use this for double-hashing in our Bloom Filter implementation.

#ifndef MURMUR_HASH3_HPP
#define MURMUR_HASH3_HPP

#include <cstdint>
#include <cstddef>

namespace quantamental {

/**
 * MurmurHash3_x64_128
 * 
 * Produces a 128-bit hash (two 64-bit values) optimized for x64 platforms.
 * 
 * @param key   Pointer to data to hash
 * @param len   Length of data in bytes
 * @param seed  Hash seed (use 0 for consistency)
 * @param out   Output array of two uint64_t values [h1, h2]
 */
void MurmurHash3_x64_128(const void* key, int len, uint32_t seed, void* out);

} // namespace quantamental

#endif // MURMUR_HASH3_HPP