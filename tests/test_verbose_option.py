import os
os.environ.setdefault('QT_QPA_PLATFORM', 'offscreen')
import pytest
from tests import PYSIDE6_AVAILABLE
if not PYSIDE6_AVAILABLE:
    pytest.skip("PySide6 not available", allow_module_level=True)
from PySide6.QtCore import QCoreApplication, QCommandLineParser, QCommandLineOption


def test_verbose_option_parsed():
    app = QCoreApplication.instance()
    if app is None:
        app = QCoreApplication([])
    parser = QCommandLineParser()
    parser.setApplicationDescription("Beya_Waifu")
    parser.addHelpOption()
    parser.addOption(QCommandLineOption("max-threads", "Override global thread limit", "count"))
    verbose_opt = QCommandLineOption(["verbose"], "Enable verbose debug logging")
    parser.addOption(verbose_opt)
    parser.process(["test", "--verbose"])
    assert parser.isSet("verbose")
