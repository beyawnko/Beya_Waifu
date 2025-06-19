"""Test package initialization."""
import warnings
from pathlib import Path

try:
    import PySide6  # noqa: F401  # type: ignore
    PYSIDE6_AVAILABLE = True
except Exception:
    PYSIDE6_AVAILABLE = False
    warnings.warn(
        "PySide6 could not be imported. GUI tests will be skipped."
        " Install it via 'pip install -r requirements.txt' to run them."
    )

LIQUIDGLASS_QSB = (
    Path(__file__).resolve().parents[1]
    / "Waifu2x-Extension-QT"
    / "shaders"
    / "liquidglass.frag.qsb"
)
LIQUIDGLASS_QSB_AVAILABLE = LIQUIDGLASS_QSB.exists()
if not LIQUIDGLASS_QSB_AVAILABLE:
    warnings.warn(
        "Missing shaders/liquidglass.frag.qsb. Liquid Glass tests will be skipped."
    )
