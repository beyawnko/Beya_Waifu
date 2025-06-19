"""Test package initialization."""
import warnings

try:
    import PySide6  # noqa: F401  # type: ignore
    PYSIDE6_AVAILABLE = True
except Exception:
    PYSIDE6_AVAILABLE = False
    warnings.warn(
        "PySide6 could not be imported. GUI tests will be skipped."
        " Install it via 'pip install -r requirements.txt' to run them."
    )
