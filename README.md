# Quantamental Equity Forecasting System

A production-grade ML system for equity return prediction, combining Python research with high-performance C++ inference.
---

## Project Overview

This project demonstrates **ML Systems Engineering** capability by building an end-to-end equity forecasting system:

- **Layer 1:** Data Ingestion & Feature Engineering (Python + C++)
- **Layer 2:** ML Research & Training (PyTorch) — *Future*
- **Layer 3:** Production Inference (C++ with libtorch) — *Future*
- **Layer 4:** Explainability & Uncertainty — *Future*

### Key Technical Highlights

| Component | Technology | Purpose |
|-----------|------------|---------|
| Data Pipeline | Python, yfinance, Parquet | Fetch and store market data |
| Deduplication | C++ Bloom Filter | Memory-efficient duplicate detection |
| Feature Engineering | pandas, numpy | Technical indicators (SMA, RSI, MACD) |
| Python-C++ Bridge | pybind11 | Expose C++ to Python |

---

## Project Structure

```
quantamental-forecasting/
├── configs/
│   └── default.yaml          # Central configuration
├── src/
│   ├── __init__.py
│   ├── data/
│   │   ├── __init__.py
│   │   └── fetcher.py        # Yahoo Finance data fetcher
│   ├── features/
│   │   ├── __init__.py
│   │   └── technical.py      # Technical indicators
│   └── utils/
│       ├── __init__.py
│       ├── config.py         # Configuration loader
│       └── logger.py         # Logging utility
├── cpp/
│   ├── include/
│   │   └── bloom_filter.hpp  # Bloom Filter public interface
│   ├── src/
│   │   ├── bloom_filter.cpp  # Bloom Filter implementation
│   │   ├── murmur_hash3.hpp  # Hash function header
│   │   ├── murmur_hash3.cpp  # Hash function implementation
│   │   └── bindings.cpp      # pybind11 Python bindings
│   ├── tests/
│   │   └── test_bloom_filter.cpp
│   ├── build/
│   └── CMakeLists.txt
├── data/
│   ├── raw/                  # Downloaded OHLCV data
│   └── processed/            # Computed features
└── README.md
```

---

## Installation

### Prerequisites

- Python 3.11+
- CMake 3.18+
- C++17 compiler (GCC 9+ or Clang 10+)
- Miniconda/Anaconda

### Environment Setup

```bash
# Create conda environment
conda create -n quant_sys python=3.11 -y
conda activate quant_sys

# Install Python dependencies
pip install numpy pandas yfinance pyyaml pybind11

# Build C++ components
cd cpp/build
cmake ..
make -j$(nproc)

# The Python module will be built as: quantamental.cpython-311-x86_64-linux-gnu.so
```

---

## Usage

### Python: Load Configuration

```python
from src.utils import get_config, get_logger

config = get_config()
logger = get_logger(__name__)

tickers = config['data']['tickers']
logger.info(f"Loaded {len(tickers)} tickers")
```

### Python: Fetch Market Data

```python
from src.data import DataFetcher

fetcher = DataFetcher()
data = fetcher.fetch_all()
fetcher.save_to_parquet(data, "sp500_data.parquet")
```

### Python: Compute Features

```python
from src.features import TechnicalFeatures

tf = TechnicalFeatures()
features = tf.compute_all(ohlcv_df)
```

### Python: Use Bloom Filter

```python
from quantamental import BloomFilter

bf = BloomFilter(expected_elements=100000, false_positive_rate=0.01)
bf.insert("AAPL:2024-01-15")
print("AAPL:2024-01-15" in bf)  # True
print("MSFT:2024-01-15" in bf)  # False (probably)
```

### C++: Run Tests

```bash
cd cpp/build
./test_bloom_filter
```

---

## Implementation Status

### Completed ✅

| File | Description |
|------|-------------|
| `configs/default.yaml` | Configuration with S&P 500 tickers, date ranges, feature params |
| `src/utils/logger.py` | Logging utility with formatted output |
| `src/utils/config.py` | YAML config loader with caching |
| `src/features/__init__.py` | Package exports |
| `src/__init__.py` | Package version |
| `cpp/include/bloom_filter.hpp` | Bloom Filter class interface |
| `cpp/src/bloom_filter.cpp` | Bloom Filter implementation |
| `cpp/src/murmur_hash3.hpp` | MurmurHash3 header |
| `cpp/src/murmur_hash3.cpp` | MurmurHash3 implementation |
| `cpp/src/bindings.cpp` | pybind11 Python bindings |

### In Progress 🔧

| File | Description |
|------|-------------|
| `cpp/CMakeLists.txt` | CMake build configuration |
| `cpp/tests/test_bloom_filter.cpp` | C++ unit tests |
| `src/utils/__init__.py` | Utils package exports |
| `src/data/__init__.py` | Data package exports |
| `src/data/fetcher.py` | yfinance data fetcher |
| `src/features/technical.py` | Technical indicators |

---

## Architecture Decisions

### Why Bloom Filter in C++?

- **Memory efficiency:** ~1.2 MB for 1M items vs ~50 MB for Python set
- **Speed:** O(k) lookup where k ≈ 7 hash functions
- **Systems showcase:** Demonstrates C++ proficiency for ML Systems roles

### Why Parquet for Data Storage?

- **Columnar format:** Fast for analytics queries
- **Compression:** ~10x smaller than CSV
- **Type preservation:** No type inference on load

### Why MurmurHash3?

- **Fast:** Few cycles per byte
- **Well-distributed:** Passes SMHasher tests
- **128-bit output:** Enables double-hashing technique

---

## Third-Party Code

- **MurmurHash3**: Public domain implementation by Austin Appleby
  - Source: https://github.com/aappleby/smhasher
  - Used for: Bloom Filter hash computation

---

## Future Work (Layers 2-4)

- [ ] Horizon-Aware MLP model (PyTorch)
- [ ] Walk-Forward Validation with Purge/Embargo
- [ ] C++ Inference Server (libtorch)
- [ ] Uncertainty Quantification (MC Dropout)
- [ ] SHAP Explainability
- [ ] Streamlit Dashboard

---

## License


---

## Contact

Jongsul Han — TUM Computer Science