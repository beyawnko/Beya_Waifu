import os
os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")
from pathlib import Path
import pytest
from tests import PYSIDE6_AVAILABLE

if not PYSIDE6_AVAILABLE:
    pytest.skip("PySide6 not available", allow_module_level=True)

from PySide6.QtWidgets import QApplication, QWidget
from PySide6.QtCore import QFile
from PySide6.QtUiTools import QUiLoader


@pytest.fixture(scope="session")
def qapp():
    os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")
    app = QApplication.instance()
    if app is None:
        app = QApplication([])
    return app


def test_mainwindow_ui_load(qapp):
    ui_path = Path(__file__).resolve().parents[1] / "Waifu2x-Extension-QT" / "mainwindow.ui"
    assert ui_path.exists()
    loader = QUiLoader()
    ui_file = QFile(str(ui_path))
    assert ui_file.open(QFile.ReadOnly)
    window = loader.load(ui_file)
    ui_file.close()
    assert window is not None
    assert window.findChild(QWidget, "tabWidget") is not None
    assert window.findChild(QWidget, "pushButton_Start") is not None
