import logging

def get_logger(name):
    """
    Creates a configured logger with timestamp, level, and module name formatting.

    Args:
        name (str): Module name (typically __name__ from the calling module)

    Returns:
        logging.Logger: configured logger instance

    Example:
        >>> logger = get_logger(__name__)
        >>> logger.info("Starting data fetch")
        2025-12-20 10:30:45 - INFO - my_module - Starting data fetch
    """
    # Get or create a logger with the specified name
    logger = logging.getLogger(name)

    # Only add handlers if the logger doesn't already have them
    # This prevents duplicate log messages
    if not logger.handlers:
        # Create a handler that outputs to console (stdout/stderr)
        console_handler = logging.StreamHandler()

        # Create a formatter that defines the log message format
        formatter = logging.Formatter(
            fmt='%(asctime)s - %(levelname)s - %(name)s - %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S'
        )

        # Attach the formatter to the handler
        console_handler.setFormatter(formatter)

        # Attach the handler to the logger
        logger.addHandler(console_handler)

        # Set the minimum logging level (INFO means DEBUG messages won't show)
        logger.setLevel(logging.INFO)

    return logger