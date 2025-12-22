# Quantamental Equity Forecasting System

<p align="center">
  <strong>A production-grade ML system combining Python research with high-performance C++ inference</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Python-3.11+-blue.svg" alt="Python">
  <img src="https://img.shields.io/badge/C++-20-00599C.svg" alt="C++">
  <img src="https://img.shields.io/badge/PyTorch-2.0+-red.svg" alt="PyTorch">
  <img src="https://img.shields.io/badge/Status-Layer%201%20Complete-green.svg" alt="Status">
</p>

---

## Overview

The **Quantamental Equity Forecasting System** is a portfolio project demonstrating **ML Systems Engineering** — the ability to bridge machine learning research with production-grade systems. The project predicts equity returns across multiple time horizons by combining quantitative features with technical indicators, deploying predictions via a high-performance C++ inference layer.

This project targets the intersection of ML research and systems engineering, a rare and highly demanded skill set at quantitative finance firms and Big Tech AI infrastructure teams.

### Key Differentiators

| Capability | Description |
|------------|-------------|
| **ML Systems Engineering** | Python research → C++ production pipeline |
| **High-Performance Computing** | Custom C++ Bloom Filter with pybind11 bindings |
| **Production Data Pipeline** | Efficient data ingestion with Parquet storage |
| **Comprehensive Feature Engineering** | Technical indicators (SMA, EMA, RSI, MACD, Volatility) |

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    QUANTAMENTAL FORECASTING SYSTEM                          │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ LAYER 1: Data Ingestion & Feature Engineering        ✅ COMPLETE    │   │
│  │  • Market data fetching (yfinance)                                  │   │
│  │  • Technical indicators (SMA, EMA, RSI, MACD, Volatility)          │   │
│  │  • C++ Bloom Filter for deduplication (pybind11)                   │   │
│  │  • Parquet storage with snappy compression                          │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                      ↓                                      │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ LAYER 2: ML Research & Training                      🔜 PLANNED     │   │
│  │  • Horizon-Aware MLP (PyTorch)                                      │   │
│  │  • Per-stock Partial Pooling Heads (Ridge)                         │   │
│  │  • Walk-Forward Validation with Purge/Embargo                      │   │
│  │  • Model export (TorchScript)                                       │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                      ↓                                      │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ LAYER 3: Production Inference (C++)                  🔜 PLANNED     │   │
│  │  • libtorch model loading                                           │   │
│  │  • Multi-threaded inference with OpenMP                            │   │
│  │  • Memory pool optimization                                         │   │
│  │  • REST API (cpp-httplib)                                          │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                      ↓                                      │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ LAYER 4: Intelligence & UI                           🔜 PLANNED     │   │
│  │  • Uncertainty Quantification (Monte Carlo Dropout)                │   │
│  │  • SHAP Explainability                                              │   │
│  │  • Streamlit Dashboard                                              │   │
│  │  • Risk Management (VaR, CVaR)                                     │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Project Structure

```
quantamental/
├── configs/
│   └── default.yaml              # Central configuration (tickers, dates, params)
│
├── src/
│   ├── __init__.py
│   ├── quantamental.cpython-311-x86_64-linux-gnu.so  # C++ Python module
│   │
│   ├── data/
│   │   ├── __init__.py
│   │   └── fetcher.py            # Yahoo Finance data fetcher
│   │
│   ├── features/
│   │   ├── __init__.py
│   │   └── technical.py          # Technical indicators
│   │
│   └── utils/
│       ├── __init__.py
│       ├── config.py             # Configuration loader
│       └── logger.py             # Logging utility
│
├── cpp/
│   ├── include/
│   │   └── bloom_filter.hpp      # Bloom Filter public interface
│   │
│   ├── src/
│   │   ├── bloom_filter.cpp      # Bloom Filter implementation
│   │   ├── murmur_hash3.hpp      # MurmurHash3 header
│   │   ├── murmur_hash3.cpp      # MurmurHash3 implementation
│   │   └── bindings.cpp          # pybind11 Python bindings
│   │
│   ├── tests/
│   │   └── test_bloom_filter.cpp # C++ unit tests
│   │
│   ├── build/                    # CMake build directory
│   └── CMakeLists.txt            # Build configuration
│
├── data/
│   ├── raw/                      # Downloaded OHLCV data
│   └── processed/                # Computed features
│
├── .gitignore
└── README.md
```

---

## Installation

### Prerequisites

- **Python 3.11+** with conda/miniconda
- **CMake 3.15+**
- **C++20 compiler** (GCC 10+ or Clang 12+)
- **Git**

### Step 1: Clone Repository

```bash
git clone https://github.com/yourusername/quantamental.git
cd quantamental
```

### Step 2: Create Conda Environment

```bash
conda create -n quant_sys python=3.11 -y
conda activate quant_sys

# Install Python dependencies
pip install numpy pandas yfinance pyyaml pybind11 torch
```

### Step 3: Build C++ Components

```bash
cd cpp/build
cmake ..
make -j$(nproc)
cmake --install .
cd ../..
```

### Step 4: Verify Installation

```bash
# Test Python modules
python -c "from src.utils import get_config; print('Config loaded')"
python -c "from src.data import DataFetcher; print('DataFetcher ready')"
python -c "from src.features import TechnicalFeatures; print('TechnicalFeatures ready')"
python -c "from quantamental import BloomFilter; print('BloomFilter ready')"
```

---

## Usage

### Configuration

All parameters are centralized in `configs/default.yaml`:

```yaml
data:
  tickers: ['AAPL', 'MSFT', 'GOOGL', ...]  # S&P 500 constituents
  start_date: "2007-01-01"
  end_date: null  # null = today

features:
  horizons: [1, 5, 10, 20]      # Forecast horizons (days)
  sma_windows: [10, 20, 50, 200]
  rsi_period: 14

bloom_filter:
  expected_elements: 100000
  false_positive_rate: 0.01
```

### Fetching Market Data

```python
from src.data import DataFetcher

# Initialize with config defaults (S&P 500, from 2007)
fetcher = DataFetcher()

# Or customize
fetcher = DataFetcher(
    tickers=['AAPL', 'MSFT', 'GOOGL'],
    start_date='2020-01-01',
    end_date='2024-01-01'
)

# Fetch data
data = fetcher.fetch_all()

# Save to Parquet (compressed, efficient)
fetcher.save_to_parquet(data, 'market_data.parquet')

# Load later
df = fetcher.load_from_parquet('market_data.parquet')
```

### Computing Technical Features

```python
from src.features import TechnicalFeatures
import yfinance as yf

# Get OHLCV data
df = yf.Ticker('AAPL').history(start='2020-01-01', end='2024-01-01')

# Compute all technical indicators
tf = TechnicalFeatures()
features = tf.compute_all(df)

print(features.columns.tolist())
# ['return_1d', 'return_5d', 'return_10d', 'return_20d',
#  'sma_10', 'sma_20', 'sma_50', 'sma_200',
#  'ema_10', 'ema_20', 'ema_50', 'ema_200',
#  'rsi_14', 'macd', 'macd_signal', 'macd_hist',
#  'volatility_10d', 'volatility_20d', ...]
```

### Using the Bloom Filter (C++)

```python
from quantamental import BloomFilter

# Create filter for 100k items with 1% false positive rate
bf = BloomFilter(expected_elements=100000, false_positive_rate=0.01)

# Insert items
bf.insert("AAPL:2024-01-15:price")
bf.insert("MSFT:2024-01-15:price")

# Check membership (O(k) where k = 7)
print("AAPL:2024-01-15:price" in bf)  # True
print("GOOGL:2024-01-15:price" in bf)  # False (probably)

# Batch operations
bf.insert_batch(["key1", "key2", "key3"])
new_indices = bf.filter_new(["key1", "key4", "key5"])  # Returns [1, 2]

# Statistics
stats = bf.get_stats()
print(f"Insertions: {stats.num_insertions}")
print(f"Fill ratio: {stats.fill_ratio:.2%}")
print(f"Est. FPR: {stats.estimated_fpr:.4f}")

# Persistence
bf.save_to_file("bloom_state.bin")
bf_loaded = BloomFilter.load_from_file("bloom_state.bin")
```

---

## Layer 1: Technical Deep Dive

### C++ Bloom Filter

The Bloom Filter is implemented in C++ for maximum performance and exposed to Python via pybind11.

**Why C++ for Bloom Filter?**

| Aspect | Python Set | C++ Bloom Filter |
|--------|------------|------------------|
| Memory (1M items) | ~50 MB | ~1.2 MB |
| Lookup time | O(1) avg | O(k) = O(7) |
| False positives | None | ~1% (configurable) |
| False negatives | None | **None** (guaranteed) |

**Optimal Parameters:**
- Bit array size: `m = -n * ln(p) / (ln(2))²`
- Hash functions: `k = (m/n) * ln(2)`
- For n=100,000, p=0.01: m ≈ 958,506 bits (~120 KB), k = 7

**Double Hashing Technique:**

Instead of k independent hash functions, we use MurmurHash3's 128-bit output:
```
h_i(x) = (h1(x) + i × h2(x)) mod m,  for i = 0, 1, ..., k-1
```

### Data Pipeline

```
Yahoo Finance API
       ↓
  DataFetcher.fetch_batch()
       ↓
  Dict[ticker, DataFrame]
       ↓
  save_to_parquet() ──→ Parquet file (snappy compression)
       ↓
  TechnicalFeatures.compute_all()
       ↓
  Feature DataFrame (returns, SMA, EMA, RSI, MACD, volatility)
```

**Why Parquet?**
- Columnar format: 10x faster for analytics
- Snappy compression: ~10x smaller than CSV
- Type preservation: No inference on load
- Industry standard: Used by Spark, pandas, DuckDB

### Technical Indicators Implemented

| Indicator | Formula | Use Case |
|-----------|---------|----------|
| **Returns** | `(P_t - P_{t-n}) / P_{t-n}` | Momentum signals |
| **SMA** | `mean(P_{t-n:t})` | Trend identification |
| **EMA** | Exponentially weighted mean | Recent trend emphasis |
| **RSI** | `100 - 100/(1 + RS)` | Overbought/oversold |
| **MACD** | `EMA(12) - EMA(26)` | Trend momentum |
| **Volatility** | `std(returns, n)` | Risk measurement |

---

## Development Progress

### ✅ Completed (Layer 1)

- [x] Project structure and configuration system
- [x] Logging utility with formatted output
- [x] YAML configuration loader with caching
- [x] Yahoo Finance data fetcher with batch support
- [x] Parquet save/load functionality
- [x] Technical indicators (SMA, EMA, RSI, MACD, Volatility)
- [x] C++ Bloom Filter implementation
- [x] MurmurHash3 integration (public domain)
- [x] pybind11 Python bindings
- [x] CMake build system

### 🔜 Planned (Layer 2: ML Research)

- [ ] Horizon-Aware MLP architecture (PyTorch)
- [ ] Per-stock Partial Pooling Heads (Ridge regression)
- [ ] Walk-Forward Validation with Purge/Embargo
- [ ] Information Coefficient (IC) evaluation
- [ ] TorchScript model export

### 🔜 Planned (Layer 3: Production Inference)

- [ ] C++ inference server with libtorch
- [ ] REST API with cpp-httplib
- [ ] OpenMP multi-threaded batch inference
- [ ] Memory pooling for zero-allocation inference
- [ ] SIMD optimizations (AVX2)

### 🔜 Planned (Layer 4: Intelligence & UI)

- [ ] Uncertainty Quantification (Monte Carlo Dropout)
- [ ] SHAP explainability for predictions
- [ ] Streamlit dashboard
- [ ] Risk metrics (VaR, CVaR)
- [ ] Email/Telegram notifications

---

## Performance Benchmarks

### Bloom Filter (C++)

| Operation | Time (μs) | Throughput |
|-----------|-----------|------------|
| Insert (single) | ~0.5 | 2M ops/sec |
| Query (single) | ~0.3 | 3M ops/sec |
| Insert batch (1000) | ~400 | 2.5M ops/sec |

### Data Pipeline

| Operation | Time | Notes |
|-----------|------|-------|
| Fetch 500 tickers (5 years) | ~60s | Yahoo Finance rate limited |
| Save to Parquet (500 tickers) | ~2s | With snappy compression |
| Load from Parquet | ~0.5s | Columnar advantage |
| Compute all features (1 ticker) | ~10ms | Vectorized pandas |

---

## Third-Party Code

- **MurmurHash3**: Public domain implementation by Austin Appleby
  - Source: https://github.com/aappleby/smhasher
  - Used for: Bloom Filter hash computation

---

## Design Decisions

### Why Python + C++?

| Component | Language | Rationale |
|-----------|----------|-----------|
| Data fetching | Python | yfinance library, rapid development |
| Feature engineering | Python | pandas vectorization, flexibility |
| Bloom Filter | C++ | Memory efficiency, performance critical |
| ML training | Python | PyTorch ecosystem |
| Production inference | C++ | Latency requirements (<10ms) |

### Why Not...?

| Alternative | Reason Not Used |
|-------------|-----------------|
| TensorFlow | PyTorch better for research-to-production |
| Redis for dedup | Overkill, external dependency |
| CSV storage | 10x larger, slower, no types |
| Pure Python Bloom | 40x more memory |

---

## Contributing

This is a portfolio project, but suggestions are welcome. Please open an issue for:
- Bug reports
- Feature suggestions
- Performance improvements

---

## License

MIT License

---

## Author

**Jongsul Han**  
Computer Science @ Technical University of Munich (TUM)  
Target: ML Systems Engineer positions at Nvidia, ARM, Quantitative Firms

---

## Acknowledgments

- Austin Appleby for MurmurHash3
- pybind11 team for seamless C++/Python integration
- Yahoo Finance for free market data API