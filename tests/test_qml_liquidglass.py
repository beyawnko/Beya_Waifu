import os
os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")
from pathlib import Path
import pytest
from tests import PYSIDE6_AVAILABLE, LIQUIDGLASS_QSB_AVAILABLE
if not PYSIDE6_AVAILABLE:
    pytest.skip("PySide6 not available", allow_module_level=True)
if not LIQUIDGLASS_QSB_AVAILABLE:
    pytest.skip("liquidglass.frag.qsb not built", allow_module_level=True)
from PySide6.QtCore import QUrl
from PySide6.QtQml import QQmlEngine, QQmlComponent
from PySide6.QtWidgets import QApplication


@pytest.fixture(scope="session")
def qapp():
    os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")
    app = QApplication.instance()
    if app is None:
        app = QApplication([])
    return app


def test_liquidglass_qml_load(qapp):
    qml_path = Path(__file__).resolve().parents[1] / "Waifu2x-Extension-QT" / "qml" / "LiquidGlass.qml"
    assert qml_path.exists()
    engine = QQmlEngine()
    component = QQmlComponent(engine, QUrl.fromLocalFile(str(qml_path)))
    if component.status() != QQmlComponent.Ready:
        errors = [err.toString() for err in component.errors()]
        pytest.fail("QML failed to load: " + "\n".join(errors))
    obj = component.create()
    assert obj is not None
