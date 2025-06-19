import os
import pytest
from tests import PYSIDE6_AVAILABLE

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

if not PYSIDE6_AVAILABLE:
    pytest.skip("PySide6 not available", allow_module_level=True)

from PySide6.QtWidgets import QApplication, QStyleFactory

def test_qstyle_fallback():
    app = QApplication.instance() or QApplication([])
    style = QStyleFactory.create("macos")
    if not style:
        style = QStyleFactory.create("Fusion")
    if style:
        app.setStyle(style)
    assert QApplication.style() is not None
