#include <cstddef>
#include <cstdint>

namespace quantamental {
    void MurmurHash3_x64_128(const void* key, size_t len, uint32_t seed, uint64_t* out);
}