// bindings.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // For std::vector, std::optional, std::string
#include <pybind11/operators.h>
#include "bloom_filter.hpp"

namespace py = pybind11;

PYBIND11_MODULE(quantamental, m) {
    m.doc() = "Quantamental C++ library - Bloom Filter implementation";

    // ========================================================================
    // Expose BloomFilterStats struct
    // ========================================================================
    py::class_<quantamental::BloomFilter::BloomFilterStats>(m, "BloomFilterStats")
        .def_readonly("num_insertions", &quantamental::BloomFilter::BloomFilterStats::num_insertions)
        .def_readonly("num_queries", &quantamental::BloomFilter::BloomFilterStats::num_queries)
        .def_readonly("bit_array_size", &quantamental::BloomFilter::BloomFilterStats::bit_array_size)
        .def_readonly("bits_set", &quantamental::BloomFilter::BloomFilterStats::bits_set)
        .def_readonly("num_hash_functions", &quantamental::BloomFilter::BloomFilterStats::num_hash_functions)
        .def_readonly("fill_ratio", &quantamental::BloomFilter::BloomFilterStats::fill_ratio)
        .def_readonly("estimated_fpr", &quantamental::BloomFilter::BloomFilterStats::estimated_fpr)
        .def("__repr__", [](const quantamental::BloomFilter::BloomFilterStats& s) {
            return "<BloomFilterStats: insertions=" + std::to_string(s.num_insertions) +
                   ", fill_ratio=" + std::to_string(s.fill_ratio) +
                   ", fpr=" + std::to_string(s.estimated_fpr) + ">";
        });

    // ========================================================================
    // Expose BloomFilter class
    // ========================================================================
    py::class_<quantamental::BloomFilter>(m, "BloomFilter")
        // Constructors
        .def(py::init<size_t, double>(),
             py::arg("expected_elements"),
             py::arg("false_positive_rate") = 0.01,
             "Create a Bloom filter for expected number of elements and target FPR")
        .def(py::init<size_t, uint32_t>(),
             py::arg("num_bits"),
             py::arg("num_hashes"),
             "Create a Bloom filter with specific bit array size and hash count (for testing)")

        // Core operations
        .def("insert", &quantamental::BloomFilter::insert,
             py::arg("key"),
             "Insert a key into the Bloom filter")
        .def("possibly_contains", &quantamental::BloomFilter::possibly_contains,
             py::arg("key"),
             "Check if key might be in the filter (may have false positives)")
        .def("insert_and_check", &quantamental::BloomFilter::insert_and_check,
             py::arg("key"),
             "Insert key and return True if it was new")
        .def("__contains__", &quantamental::BloomFilter::possibly_contains,
             "Support 'key in filter' syntax")

        // Batch operations
        .def("insert_batch", &quantamental::BloomFilter::insert_batch,
             py::arg("keys"),
             "Insert multiple keys at once")
        .def("filter_new", &quantamental::BloomFilter::filter_new,
             py::arg("keys"),
             "Return indices of keys not in the filter")

        // Statistics
        .def("get_stats", &quantamental::BloomFilter::get_stats,
             "Get detailed statistics about the filter")
        .def("fill_ratio", &quantamental::BloomFilter::fill_ratio,
             "Get the ratio of set bits to total bits")
        .def("estimated_false_positive_rate", &quantamental::BloomFilter::estimated_false_positive_rate,
             "Estimate current false positive rate based on fill ratio")
        .def("size_bits", &quantamental::BloomFilter::size_bits,
             "Get size in bits")
        .def("size_bytes", &quantamental::BloomFilter::size_bytes,
             "Get size in bytes")
        .def("num_insertions", &quantamental::BloomFilter::num_insertions,
             "Get number of insertions performed")

        // Persistence
        .def("save_to_file", &quantamental::BloomFilter::save_to_file,
             py::arg("filepath"),
             "Save the Bloom filter to a binary file")
        .def("clear", &quantamental::BloomFilter::clear,
             "Clear all bits and reset counters")

        // Static methods
        .def_static("load_from_file", &quantamental::BloomFilter::load_from_file,
                    py::arg("filepath"),
                    "Load a Bloom filter from a binary file")
        .def_static("optimal_num_bits", &quantamental::BloomFilter::optimal_num_bits,
                    py::arg("n"), py::arg("p"),
                    "Calculate optimal bit array size for n elements and p false positive rate")
        .def_static("optimal_num_hashes", &quantamental::BloomFilter::optimal_num_hashes,
                    py::arg("m"), py::arg("n"),
                    "Calculate optimal number of hash functions for m bits and n elements")

        // Python-friendly representation
        .def("__repr__", [](const quantamental::BloomFilter& bf) {
            return "<BloomFilter: " + std::to_string(bf.size_bits()) + " bits, " +
                   std::to_string(bf.num_insertions()) + " insertions>";
        })
        .def("__len__", &quantamental::BloomFilter::num_insertions,
             "Return number of insertions (approximate set size)");

    // ========================================================================
    // Module-level convenience functions
    // ========================================================================
    m.def("optimal_bits", &quantamental::BloomFilter::optimal_num_bits,
          py::arg("expected_elements"),
          py::arg("false_positive_rate"),
          "Calculate optimal bit array size");

    m.def("optimal_hashes", &quantamental::BloomFilter::optimal_num_hashes,
          py::arg("num_bits"),
          py::arg("expected_elements"),
          "Calculate optimal number of hash functions");
}