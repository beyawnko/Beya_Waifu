import os
from PySide6.QtCore import QDirIterator, QDir

def test_qdir_iterator_recursive(tmp_path):
    base = tmp_path / "root"
    sub = base / "sub1" / "sub2"
    sub.mkdir(parents=True)
    f1 = base / "sub1" / "file1.txt"
    f1.write_text("a")
    f2 = sub / "file2.txt"
    f2.write_text("b")

    found = set()
    it = QDirIterator(str(base), QDir.Files, QDirIterator.Subdirectories)
    while it.hasNext():
        found.add(os.path.relpath(it.next(), str(base)))

    assert found == {"sub1/file1.txt", "sub1/sub2/file2.txt"}

