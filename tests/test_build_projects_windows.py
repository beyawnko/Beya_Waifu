import os
import subprocess
from pathlib import Path


def run_build(tmp_path: Path, uname_output: str) -> None:
    repo_root = Path(__file__).resolve().parents[1]
    script_path = repo_root / 'build_projects.sh'
    tmp_script = tmp_path / 'build_projects.sh'
    tmp_script.write_text(script_path.read_text())
    (tmp_path / 'Waifu2x-Extension-QT').mkdir()
    (tmp_path / 'Waifu2x-Extension-QT-Launcher').mkdir()
    (tmp_path / 'realesrgan-ncnn-vulkan' / 'windows').mkdir(parents=True)
    (tmp_path / 'realcugan-ncnn-vulkan' / 'windows').mkdir(parents=True)
    (tmp_path / 'realesrgan-ncnn-vulkan' / 'windows' / 'realesrgan.exe').write_text('')
    (tmp_path / 'realesrgan-ncnn-vulkan' / 'windows' / 'realesrgan.dll').write_text('')
    (tmp_path / 'realcugan-ncnn-vulkan' / 'windows' / 'realcugan.exe').write_text('')
    (tmp_path / 'realcugan-ncnn-vulkan' / 'windows' / 'realcugan.dll').write_text('')
    bin_dir = tmp_path / 'bin'
    bin_dir.mkdir()
    for name in ('qmake', 'make', 'cmake'):
        f = bin_dir / name
        f.write_text('#!/bin/sh\nexit 0\n')
        f.chmod(0o755)
    uname_stub = bin_dir / 'uname'
    uname_stub.write_text(f'#!/bin/sh\necho {uname_output}\n')
    uname_stub.chmod(0o755)
    env = os.environ.copy()
    env['PATH'] = str(bin_dir) + os.pathsep + env.get('PATH', '')
    subprocess.run(['bash', str(tmp_script)], cwd=tmp_path, check=True, env=env)
    qt_dir = tmp_path / 'Waifu2x-Extension-QT'
    launcher_dir = tmp_path / 'Waifu2x-Extension-QT-Launcher'
    for fname in ('realesrgan.exe', 'realesrgan.dll', 'realcugan.exe', 'realcugan.dll'):
        assert (qt_dir / fname).exists()
        assert (launcher_dir / fname).exists()


def test_copy_on_msys(tmp_path):
    run_build(tmp_path, 'MSYS_NT-10.0')


def test_copy_on_mingw(tmp_path):
    run_build(tmp_path, 'MINGW64_NT-10.0')
