from typing import List
import numpy as np
import pandas as pd

from src.utils.config import get_config
from src.utils.logger import get_logger

logger = get_logger(__name__)

class TechnicalFeatures:
    def __init__(self):
        """
        Load params from config (windows, periods).
        Initializes horizons for returns, SMA windows, and RSI period.
        """
        config = get_config()

        self.horizons = config['features']['horizons']
        self.sma_windows = config['features']['sma_windows']
        self.rsi_period = config['features']['rsi_period']

        logger.info("TechnicalFeatures initialized")

    def compute_returns(self, df: pd.DataFrame, windows: List[int] = None) -> pd.DataFrame:
        """
        Compute percentage returns over different periods.
        Returns = (price_t - price_{t-n}) / price_{t-n}
        """
        if windows is None:
            windows = self.horizons

        result = pd.DataFrame(index=df.index)
        close = df['Close']

        for w in windows:
            result[f'return_{w}d'] = close.pct_change(periods=w)

        return result

    def compute_sma(self, df: pd.DataFrame) -> pd.DataFrame:
        """
        Compute Simple Moving Averages for configured windows.
        SMA = mean of last n closing prices.
        """
        result = pd.DataFrame(index=df.index)
        close = df['Close']

        for w in self.sma_windows:
            result[f'sma_{w}'] = close.rolling(window=w).mean()

        return result

    def compute_ema(self, df: pd.DataFrame) -> pd.DataFrame:
        """
        Compute Exponential Moving Averages for configured windows.
        EMA gives more weight to recent prices.
        """
        result = pd.DataFrame(index=df.index)
        close = df['Close']

        for w in self.sma_windows:
            result[f'ema_{w}'] = close.ewm(span=w, adjust=False).mean()

        return result

    def compute_rsi(self, df: pd.DataFrame, period: int = None) -> pd.Series:
        """
        Compute Relative Strength Index (RSI).
        RSI = 100 - (100 / (1 + RS)), where RS = avg_gain / avg_loss.
        RSI ranges 0-100; >70 overbought, <30 oversold.
        """
        if period is None:
            period = self.rsi_period

        close = df['Close']
        delta = close.diff()

        gain = delta.where(delta > 0, 0.0)
        loss = (-delta).where(delta < 0, 0.0)

        avg_gain = gain.ewm(span=period, adjust=False).mean()
        avg_loss = loss.ewm(span=period, adjust=False).mean()

        rs = avg_gain / avg_loss
        rsi = 100 - (100 / (1 + rs))
        rsi.name = f'rsi_{period}'

        return rsi

    def compute_macd(self, df: pd.DataFrame) -> pd.DataFrame:
        """
        Compute MACD (Moving Average Convergence Divergence).
        MACD line = EMA(12) - EMA(26), Signal line = EMA(9) of MACD.
        Histogram = MACD - Signal. Crossovers indicate buy/sell signals.
        """
        close = df['Close']

        ema_12 = close.ewm(span=12, adjust=False).mean()
        ema_26 = close.ewm(span=26, adjust=False).mean()

        macd_line = ema_12 - ema_26
        signal_line = macd_line.ewm(span=9, adjust=False).mean()
        histogram = macd_line - signal_line

        result = pd.DataFrame({
            'macd': macd_line,
            'macd_signal': signal_line,
            'macd_hist': histogram
        }, index=df.index)

        return result

    def compute_volatility(self, df: pd.DataFrame) -> pd.DataFrame:
        """
        Compute rolling volatility (std of returns) for configured windows.
        Higher volatility = higher risk/uncertainty.
        """
        result = pd.DataFrame(index=df.index)
        returns = df['Close'].pct_change()

        for w in self.sma_windows:
            result[f'volatility_{w}d'] = returns.rolling(window=w).std()

        return result

    def compute_volume_features(self, df: pd.DataFrame) -> pd.DataFrame:
        """
        Compute volume-based features: rolling averages and relative volume.
        Relative volume = current volume / avg volume (spike detection).
        """
        result = pd.DataFrame(index=df.index)
        volume = df['Volume']

        for w in self.sma_windows:
            vol_ma = volume.rolling(window=w).mean()
            result[f'volume_sma_{w}'] = vol_ma
            result[f'volume_ratio_{w}'] = volume / vol_ma

        return result

    def compute_all(self, df: pd.DataFrame) -> pd.DataFrame:
        """
        Compute all technical features and concatenate into single DataFrame.
        Includes: returns, SMA, EMA, RSI, MACD, volatility, volume features.
        """
        features = [
            self.compute_returns(df),
            self.compute_sma(df),
            self.compute_ema(df),
            self.compute_rsi(df).to_frame(),
            self.compute_macd(df),
            self.compute_volatility(df),
            self.compute_volume_features(df)
        ]

        result = pd.concat(features, axis=1)
        return result