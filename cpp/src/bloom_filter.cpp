#include "bloom_filter.hpp"
#include "murmur_hash3.hpp"
#include <cmath>
#include <fstream>
#include <algorithm>
#include <bit>

namespace quantamental {
    // ============================================================================
    // Static Utilities
    // ============================================================================

    size_t BloomFilter::optimal_num_bits(size_t n, double p) {
        return static_cast<size_t>(std::ceil(-static_cast<double>(n) * std::log(p) /
                                             (std::log(2.0) * std::log(2.0))));
    }

    uint32_t BloomFilter::optimal_num_hashes(size_t m, size_t n) {
        if (n == 0) return 1;

        return static_cast<uint32_t>(std::round(static_cast<double>(m) / n * std::log(2.0)));
    }

    // ============================================================================
    // Constructors
    // ============================================================================
    BloomFilter::BloomFilter(size_t expected_elements, double false_positive_rate) 
        : num_insertions_(0), num_queries_(0) {
        num_bits_ = optimal_num_bits(expected_elements, false_positive_rate);
        num_hashes_ = optimal_num_hashes(num_bits_, expected_elements);
        num_words_ = (num_bits_ + 63) / 64; // Ceiling division

        bit_array_ = std::make_unique<uint64_t[]>(num_words_);
        std::fill_n(bit_array_.get(), num_words_, 0);
    }

    BloomFilter::BloomFilter(size_t num_bits, uint32_t num_hashes) 
        : num_bits_(num_bits), num_hashes_(num_hashes),
          num_insertions_(0), num_queries_(0) {
        
        num_words_ = (num_bits_ + 63) / 64;
        bit_array_ = std::make_unique<uint64_t[]>(num_words_);
        std::fill_n(bit_array_.get(), num_words_, 0);
    }

    // ============================================================================
    // Private Helpers
    // ============================================================================
    void BloomFilter::set_bit(size_t index) {
        size_t word_index = index / 64;
        size_t bit_index = index % 64;
        bit_array_[word_index] |= (1ULL << bit_index);
    }

    bool BloomFilter::test_bit(size_t index) const {
        size_t word_index = index / 64;
        size_t bit_index = index % 64;
        return ((bit_array_[word_index] & (1ULL << bit_index)) != 0);
    }

    uint64_t BloomFilter::count_set_bits() const {
        uint64_t count = 0;
        for (size_t i = 0; i < num_words_; ++i) {
            count += std::popcount(bit_array_[i]);

        }
        return count;
    }

    std::vector<size_t> BloomFilter::compute_hashes(const void* data, size_t len) const {
        std::vector<size_t> hashes;
        hashes.reserve(num_hashes_);

        for (uint32_t i = 0; i < num_hashes_; ++i) {
            uint64_t hash[2];
            MurmurHash3_x64_128(data, len, i, hash);
            hashes.push_back(hash[0] % num_bits_);
        }
        return hashes;
    }

    
    // ============================================================================
    // Core Operations
    // ============================================================================
    void BloomFilter::insert(std::string_view key) {
        auto hashes = compute_hashes(key.data(), key.size());
        for (size_t hash : hashes) {
            set_bit(hash);
        }
        ++num_insertions_;
    }

    bool BloomFilter::possibly_contains(std::string_view key) const {
        auto hashes = compute_hashes(key.data(), key.size());
        for (size_t hash : hashes) {
            if (!test_bit(hash)) {
                return false;
            }
        }
        ++num_queries_;
        return true;
    }

    bool BloomFilter::insert_and_check(std::string_view key) {
        auto hashes = compute_hashes(key.data(), key.size());
        bool is_new = false;

        for (size_t hash : hashes) {
            if (!test_bit(hash)) {
                is_new = true;
                set_bit(hash);
            }
        }

        if (is_new) {
            ++num_insertions_;
        }
        return is_new;
    }

    // ============================================================================
    // Batch Operations
    // ============================================================================
    void BloomFilter::insert_batch(const std::vector<std::string>& keys) {
        for (const auto& key : keys) {
            insert(key);
        }
    }

    std::vector<size_t> BloomFilter::filter_new(const std::vector<std::string>& keys) const {
        std::vector<size_t> new_indices;
        for (size_t i = 0; i < keys.size(); ++i) {
            if (!possibly_contains(keys[i])) {
                new_indices.push_back(i);
            }
        }
        return new_indices;
    }

    // ============================================================================
    // Statistics
    // ============================================================================

    BloomFilter::BloomFilterStats BloomFilter::get_stats() const {
        uint64_t bits_set = count_set_bits();
        double fill = static_cast<double>(bits_set) / num_bits_;
        
        return BloomFilterStats{
            num_insertions_,
            num_queries_,
            num_bits_,
            bits_set,
            num_hashes_,
            fill,
            estimated_false_positive_rate()
        };
    }

    double BloomFilter::fill_ratio() const {
        return static_cast<double>(count_set_bits()) / num_bits_;
    }

    double BloomFilter::estimated_false_positive_rate() const {
        if (num_insertions_ == 0) return 0.0;
        
        // Formula: (1 - e^(-k*n/m))^k
        double exponent = -static_cast<double>(num_hashes_ * num_insertions_) / num_bits_;
        return std::pow(1.0 - std::exp(exponent), num_hashes_);
    }

    size_t BloomFilter::size_bits() const {
        return num_bits_;
    }

    size_t BloomFilter::size_bytes() const {
        return num_words_ * sizeof(uint64_t);
    }

    uint64_t BloomFilter::num_insertions() const {
        return num_insertions_;
    }

    // ============================================================================
    // Persistence
    // ============================================================================

    bool BloomFilter::save_to_file(const std::string& filepath) const {
        std::ofstream file(filepath, std::ios::binary);
        if (!file) return false;
        
        // Write metadata
        file.write(reinterpret_cast<const char*>(&num_bits_), sizeof(num_bits_));
        file.write(reinterpret_cast<const char*>(&num_hashes_), sizeof(num_hashes_));
        file.write(reinterpret_cast<const char*>(&num_insertions_), sizeof(num_insertions_));
        file.write(reinterpret_cast<const char*>(&num_queries_), sizeof(num_queries_));
        
        // Write bit array
        file.write(reinterpret_cast<const char*>(bit_array_.get()), 
                num_words_ * sizeof(uint64_t));
        
        return file.good();
    }

    std::optional<BloomFilter> BloomFilter::load_from_file(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) return std::nullopt;
        
        // Read metadata
        size_t num_bits;
        uint32_t num_hashes;
        uint64_t num_insertions, num_queries;
        
        file.read(reinterpret_cast<char*>(&num_bits), sizeof(num_bits));
        file.read(reinterpret_cast<char*>(&num_hashes), sizeof(num_hashes));
        file.read(reinterpret_cast<char*>(&num_insertions), sizeof(num_insertions));
        file.read(reinterpret_cast<char*>(&num_queries), sizeof(num_queries));
        
        if (!file) return std::nullopt;
        
        // Create filter
        BloomFilter filter(num_bits, num_hashes);
        filter.num_insertions_ = num_insertions;
        filter.num_queries_ = num_queries;
        
        // Read bit array
        file.read(reinterpret_cast<char*>(filter.bit_array_.get()), 
                filter.num_words_ * sizeof(uint64_t));
        
        if (!file) return std::nullopt;
        
        return filter;
    }

    void BloomFilter::clear() {
        std::fill_n(bit_array_.get(), num_words_, 0);
        num_insertions_ = 0;
        num_queries_ = 0;
    }
}