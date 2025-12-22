"""
Download OHLCV market data from Yahoo Finance and save to Parquet.
"""

from pathlib import Path
from typing import List, Dict, Optional
import pandas as pd
import yfinance as yf

from src.utils.config import get_config
from src.utils.logger import get_logger

logger = get_logger(__name__)

class DataFetcher:
    """Fetch OHLCV data from Yahoo Finance and save to Parquet format."""
    
    def __init__(
            self,
            tickers: Optional[List[str]] = None,
            start_date: Optional[str] = None,
            end_date: Optional[str] = None
    ):
        """
        Initialize DataFetcher with config defaults.

        Args:
            tickers: List of ticker symbols (default: from config)
            start_date: Start date in 'YYYY-MM-DD' format (default: from config)
            end_date: End date in 'YYYY-MM-DD' format (default: from config)
        """
        # Load config
        config = get_config()

        # Set attributes with defaults from config
        self.tickers = tickers if tickers is not None else config['data']['tickers']
        self.start_date = start_date if start_date is not None else config['data']['start_date']
        self.end_date = end_date if end_date is not None else config['data']['end_date']

        # Create data directory
        raw_path = config['data']['raw_path']
        self.data_dir = Path(raw_path)
        self.data_dir.mkdir(parents=True, exist_ok=True)

        logger.info(f"DataFetcher initialized: {len(self.tickers)} tickers,"
                            f"from {self.start_date} to {self.end_date}")
        
    def fetch_single(self, ticker: str) -> Optional[pd.DataFrame]:
        """
        Fetch OHLCV data for a single ticker.

        Args:
            ticker: Ticker symbol (e.g., 'AAPL')

        Returns:
            DataFrame with OHLCV data, or None if fetch fails
        """
        try:
            logger.info(f"Fetching {ticker}...")

            # Fetch data using  yfinance
            df = yf.Ticker(ticker).history(
                start=self.start_date,
                end=self.end_date,
                auto_adjust=True
            )

            # Check if data is empty
            if df.empty:
                logger.warning(f"{ticker}: No data returned")
                return None
            
            logger.info(f"ticker: Successfully fetched {len(df)} rows")
            return df
        
        except Exception as e:
            logger.error(f"{ticker}: Failed to fetch - {str(e)}")
            return None
        
    def fetch_batch(self, tickers: Optional[List[str]] = None) -> Dict[str, pd.DataFrame]:
        """
        Fetch OHLCV data for multiple tickers.

        Args:
            tickers: List of ticker symbols (default: self.tickers)

        Returns:
            Dictionary mapping ticker symbols to DataFrames
        """
        tickers = tickers if tickers is not None else self.tickers

        logger.info(f"Fetching batch of {len(tickers)} tickers...")

        try:
            # Download all tickers at once
            data = yf.download(
                tickers,
                start=self.start_date,
                end=self.end_date,
                group_by='ticker',
                auto_adjust=True,
                progress=True
            )

            result = {}

            # handle single ticker case
            if len(tickers) == 1:
                ticker = tickers[0]
                if not data.empty:
                    result[ticker] = data
                    logger.into(f"{ticker}: Successfully fetched {len(data)} rows")
                else:
                    logger.warning(f"{ticker}: No data returned")
            else:
                # Handle multiple tickers
                for ticker in tickers:
                    try:
                        # Extract data for this ticker
                        ticker_data = data[ticker]

                        # Check if data is valid
                        if not ticker_data.empty and not ticker_data.isna().all.all():
                            result[ticker] = ticker_data
                            logger.info(f"{ticker}: Successfully fetched {len(ticker_data)} rows")
                        else:
                            logger.warning(f"{ticker}: No valid data")
                    
                    except (KeyError, Exception) as e:
                        logger.warning(f"{ticker}: Skipped - {str(e)}")

                logger.info(f"Batch fetch complete: {len(result)}/{len(tickers)} tickers succeeded")
                return result
        except Exception as e:
            logger.error(f"Batch fetch failed: {str(e)}")
            return result
        
    def fetch_all(self) -> Dict[str, pd.DataFrame]:
        """
        Fetch all tickers configured in self.tickers.

        Returns:
            Dictionary mapping ticker symbols to DataFrames
        """
        return self.fetch_batch(self.tickers)

    def save_to_parquet(self, data: Dict[str, pd.DataFrame], filename: str) -> Path:
        """
        Combine ticker DataFrames and save to Parquet.

        Args:
            data: Dictionary mapping tickers to DataFrames
            filename: Output filename (e.g., 'sp500_data.parquet')

        Returns:
            Path to saved file
        """
        logger.info(f"Saving {len(data)} tickers to Parquet...")

        # Prepare list of DataFrames
        dfs = []
        for ticker, df in data.items():
            # Add ticker column
            df_copy = df.copy()
            df_copy['ticker'] = ticker

            # Reset index to make Date a column
            df_copy = df_copy.reset_index()

            dfs.append(df_copy)

        # Concatenate all DataFrames
        combined = pd.concat(dfs, ignore_index=True)

        # Save to Parquet
        output_path = self.data_dir / filename
        combined.to_parquet(output_path, compression='snappy')

        logger.info(f"Saved {len(combined)} rows to {output_path}")
        return output_path
    
    def load_from_parquet(self, filename: str) -> pd.DataFrame:
        """
        Load data from Parquet file.

        Args:
            filename: Parquet filename to load

        Returns:
            DataFrame with loaded data
        """
        file_path = self.data_dir / filename

        logger.info(f"Loading data from {file_path}...")
        df = pd.read_parquet(file_path)

        logger.info(f"Loaded {len(df)} rows, {df['ticker'].nunique()} unique tickers")
        return df