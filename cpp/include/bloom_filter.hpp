#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>
#include <optional>
#include <memory>

#ifndef BLOOM_FILTER_HPP
#define BLOOM_FILTER_HPP

namespace quantamental {

class BloomFilter {

struct BloomFilterStats {
    uint64_t num_insertions;
    uint64_t num_queries;
    uint64_t bit_array_size;
    uint64_t bits_set;
    uint32_t num_hash_functions;
    double fill_ratio;
    double estimated_fpr;
};

public:
    // Constructors
    BloomFilter(size_t expected_elements, double false_positive_rate = 0.01);
    BloomFilter(size_t num_bits, uint32_t num_hashes);  // For testing

    // Core operations
    void insert(std::string_view key);
    bool possibly_contains(std::string_view key) const;
    bool insert_and_check(std::string_view key);  // Insert and return if was new

    // Batch operations
    void insert_batch(const std::vector<std::string>& keys);
    std::vector<size_t> filter_new(const std::vector<std::string>& keys) const;

    // Statistics
    BloomFilterStats get_stats() const;
    double fill_ratio() const;
    double estimated_false_positive_rate() const;
    size_t size_bits() const;
    size_t size_bytes() const;
    uint64_t num_insertions() const;

    // Persistence
    bool save_to_file(const std::string& filepath) const;
    static std::optional<BloomFilter> load_from_file(const std::string& filepath);
    void clear();

    // Static utilities
    static size_t optimal_num_bits(size_t n, double p);
    static uint32_t optimal_num_hashes(size_t m, size_t n);

private:
    std::unique_ptr<uint64_t[]> bit_array_;  // Bit storage
    size_t num_bits_;                         // Total bits (m)
    size_t num_words_;                        // Number of 64-bit words
    uint32_t num_hashes_;                     // Hash functions (k)
    uint64_t num_insertions_;                 // Counter
    uint64_t num_queries_;                    // Counter

    // Private methods
    std::vector<size_t> compute_hashes(const void* data, size_t len) const;
    void set_bit(size_t index);
    bool test_bit(size_t index) const;
    uint64_t count_set_bits() const;
};

}
#endif