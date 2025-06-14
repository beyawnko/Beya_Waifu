import os
os.environ.setdefault('QT_QPA_PLATFORM', 'offscreen')
from PySide6.QtCore import QProcess


def test_qprocess_failed_start():
    process = QProcess()
    process.start('/nonexistent/binary')
    assert not process.waitForStarted(500), 'Process unexpectedly started'
    assert process.error() == QProcess.FailedToStart


def test_qprocess_nonzero_exit(tmp_path):
    script = tmp_path / 'fail.sh'
    script.write_text('#!/bin/sh\nexit 2\n')
    script.chmod(0o755)

    process = QProcess()
    process.start(str(script))
    assert process.waitForStarted(1000)
    process.waitForFinished(1000)
    assert process.exitStatus() == QProcess.NormalExit
    assert process.exitCode() == 2
