from pathlib import Path
from typing import Optional
import yaml

# Module-level cache variables
_cached_config: Optional[dict] = None
_project_root: Optional[Path] = None

def find_project_root() -> Path:
    """
    Walk up directories to find project root (look for configs/ folder or .git).

    Returns:
        Path: Project root directory

    Raises:
        FileNotFoundError: If project root cannot be found

    Example:
        >>> root = find_project_root()
        >>> print(root)
        /home/hanege/quantamental/quantamental
    """
    # Start from the current file's location
    current_path = Path(__file__).resolve()

    for parent in [current_path] + list(current_path.parents):
        # Check if this directory contains .git/ or configs/
        if (parent / '.git').exists() or (parent / 'configs').exists():
            return parent
        
        # If we've checked all parents and found nothing, raise an error
        raise FileNotFoundError(
            "Could not find project root. Looking for .git/ or configs/ folder."
        )
    
def get_project_root() -> Path:
    """
    Return project root path (cached).

    Returns:
        Path: Cached project root directory

    Example:
        >>> root = get_project_root()
        >>> configs_dir = root / "configs"
    """
    global _project_root

    # If not cached yet, find and cache it
    if _project_root is None:
        _project_root = find_project_root()

    return _project_root

def load_config(name: str = "default") -> dict:
    """
    Load YAML file from configs/{name}.yaml.

    Args:
        name (str): Config name (default: "default")

    Returns:
        dict: Configuration dictionary loaded from YAML

    Raises:
        FileNotFoundError: If config file doesn't exist
        yaml.YAMLError: If YAML file is malformed

    Example:
        >>> config = load_config("default")
        >>> print(config['data']['start_date'])
        2007-01-01
    """

    # Get the project root
    root = get_project_root()

    # Construct path to config file
    config_path = root / "configs" / f"{name}.yaml"

    # check if file exists
    if not config_path.exists():
        raise FileNotFoundError(f"Config file not found: {config_path}")
    
    # Open and parse the YAML file
    with open(config_path, 'r') as f:
        config = yaml.safe_load(f)

    return config

def get_config() -> dict:
    """
    Return cached config (calls load_config if not loaded yet).

    Returns:
        dict: Cached configuration dictionary

    Example:
        >>> config = get_config()
        >>> tickers = config['data']['tickers']
        >>> print(len(tickers))
        503
    """
    global _cached_config

    # If not cached yet, load and cache it
    if _cached_config is None:
        _cached_config = load_config("default")

    return _cached_config