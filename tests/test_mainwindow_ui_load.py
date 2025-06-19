import os
os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")
from pathlib import Path
import pytest
from tests import PYSIDE6_AVAILABLE

if not PYSIDE6_AVAILABLE:
    pytest.skip("PySide6 not available", allow_module_level=True)

from PySide6.QtWidgets import QApplication, QWidget
from PySide6.QtCore import QFile, QBuffer, QByteArray
from PySide6.QtUiTools import QUiLoader
from lxml import etree


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

    with open(ui_path, "rb") as f:
        ui_bytes = f.read()

    parser = etree.XMLParser(recover=True)
    tree = etree.fromstring(ui_bytes, parser)
    fixed = etree.tostring(tree, encoding="utf-8")

    loader = QUiLoader()
    buffer = QBuffer()
    buffer.setData(QByteArray(fixed))
    assert buffer.open(QFile.ReadOnly)
    window = loader.load(buffer)
    buffer.close()

    assert window is not None

    assert qapp.platformName() == "offscreen"

    assert window.findChild(QWidget, "tabWidget") is not None
    assert window.findChild(QWidget, "pushButton_Start") is not None
    assert window.findChild(QWidget, "groupBox_FrameInterpolation") is not None
    assert window.findChild(QWidget, "groupBox_InputExt") is not None
