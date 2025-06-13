import os
import subprocess
from pathlib import Path
import urllib.request
import zipfile

BIN_DIR = Path(__file__).resolve().parent / 'bin'
REALCUGAN_URL = 'https://github.com/nihui/realcugan-ncnn-vulkan/releases/download/20220728/realcugan-ncnn-vulkan-20220728-ubuntu.zip'
REALESRGAN_URL = 'https://github.com/xinntao/Real-ESRGAN-ncnn-vulkan/releases/download/v0.2.0/realesrgan-ncnn-vulkan-v0.2.0-ubuntu.zip'


def ensure_realcugan():
    dest = BIN_DIR / 'realcugan'
    exe = dest / 'realcugan-ncnn-vulkan-20220728-ubuntu' / 'realcugan-ncnn-vulkan'
    if not exe.exists():
        dest.mkdir(parents=True, exist_ok=True)
        archive = dest / 'realcugan.zip'
        urllib.request.urlretrieve(REALCUGAN_URL, archive)
        with zipfile.ZipFile(archive, 'r') as z:
            z.extractall(dest)
        archive.unlink()
    if not os.access(exe, os.X_OK):
        exe.chmod(0o755)
    return exe


def ensure_realesrgan():
    dest = BIN_DIR / 'realesrgan'
    exe = dest / 'realesrgan-ncnn-vulkan-v0.2.0-ubuntu' / 'realesrgan-ncnn-vulkan'
    models_src = Path('realesrgan-ncnn-vulkan-20220424-windows/models')
    models_dst = exe.parent / 'models'
    if not exe.exists():
        dest.mkdir(parents=True, exist_ok=True)
        archive = dest / 'realesrgan.zip'
        urllib.request.urlretrieve(REALESRGAN_URL, archive)
        with zipfile.ZipFile(archive, 'r') as z:
            z.extractall(dest)
        archive.unlink()
    if not os.access(exe, os.X_OK):
        exe.chmod(0o755)
    if not models_dst.exists() and models_src.exists():
        subprocess.check_call(['cp', '-r', str(models_src), str(models_dst)])
    return exe


def test_realcugan(tmp_path):
    exe = ensure_realcugan()
    input_img = Path('realcugan-ncnn-vulkan SRC/images/0.jpg').resolve()
    output_img = tmp_path / 'out.png'
    result = subprocess.run([str(exe), '-i', str(input_img), '-o', str(output_img)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    assert result.returncode == 0, result.stderr.decode()
    assert output_img.exists()


def test_realesrgan(tmp_path):
    exe = ensure_realesrgan()
    input_img = Path('realesrgan-ncnn-vulkan-20220424-windows/input.jpg').resolve()
    output_img = tmp_path / 'out.png'
    result = subprocess.run([str(exe), '-i', str(input_img), '-o', str(output_img)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    assert result.returncode == 0, result.stderr.decode()
    assert output_img.exists()
