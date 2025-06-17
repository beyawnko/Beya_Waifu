import os
import subprocess
import sys
from pathlib import Path
import pytest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

@pytest.mark.skipif(sys.platform == "win32", reason="POSIX only")
def test_move_to_trash_cpp(tmp_path):
    repo_root = Path(__file__).resolve().parents[1]
    src_dir = repo_root / "Waifu2x-Extension-QT"
    fm_cpp = src_dir / "FileManager.cpp"
    main_cpp = tmp_path / "main.cpp"
    main_cpp.write_text(
        "#include <QGuiApplication>\n"
        "#include \"FileManager.h\"\n"
        "int main(int argc, char **argv) {\n"
        "    QGuiApplication app(argc, argv);\n"
        "    FileManager fm;\n"
        "    fm.moveToTrash(QString::fromLocal8Bit(argv[1]));\n"
        "    return 0;\n"
        "}\n"
    )
    exe = tmp_path / "trash_test"
    cflags = subprocess.check_output([
        "pkg-config", "--cflags", "Qt6Widgets"
    ]).decode().split()
    libs = subprocess.check_output([
        "pkg-config", "--libs", "Qt6Widgets"
    ]).decode().split()
    compile_cmd = [
        "g++",
        "-std=c++17",
        str(fm_cpp),
        str(main_cpp),
        "-o",
        str(exe),
        "-I",
        str(src_dir),
    ] + cflags + libs
    subprocess.run(compile_cmd, check=True)

    test_file = tmp_path / "deleteme.txt"
    test_file.write_text("data")
    subprocess.run([str(exe), str(test_file)], check=True)
    assert not test_file.exists()
