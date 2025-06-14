import os
import subprocess
from pathlib import Path
import urllib.request
import zipfile
import pytest
from PIL import Image
import base64

REALCUGAN_AVAILABLE = None
REALESRGAN_AVAILABLE = None

BIN_DIR = Path(__file__).resolve().parent / 'bin'
TEST_IMAGE_BASE64_PATH = Path(__file__).resolve().parent / 'test_image_base64.txt' # Contains 64x64 red png
TEST_IMAGE_PATH = Path(__file__).resolve().parent / 'test_image.png'

REALCUGAN_URL = 'https://github.com/nihui/realcugan-ncnn-vulkan/releases/download/20220728/realcugan-ncnn-vulkan-20220728-ubuntu.zip'
REALESRGAN_URL = 'https://github.com/xinntao/Real-ESRGAN-ncnn-vulkan/releases/download/v0.2.0/realesrgan-ncnn-vulkan-v0.2.0-ubuntu.zip'


def download_archive(url: str, dest: Path):
    """Download *url* to *dest* if the file does not already exist."""
    if not dest.exists():
        dest.parent.mkdir(parents=True, exist_ok=True)
        print(f"Downloading {url} to {dest}...")
        urllib.request.urlretrieve(url, dest)

def setup_test_image():
    if TEST_IMAGE_BASE64_PATH.exists():
        with open(TEST_IMAGE_BASE64_PATH, 'r') as f_b64, open(TEST_IMAGE_PATH, 'wb') as f_img:
            base64_data = f_b64.read()
            image_data = base64.b64decode(base64_data)
            f_img.write(image_data)
        print(f"Created test image at {TEST_IMAGE_PATH}")
    else:
        # This should not happen if test_image_base64.txt is committed
        print(f"Fatal: Base64 image data file not found: {TEST_IMAGE_BASE64_PATH}. Cannot create test image.")
        pytest.exit("Test image data is missing.")

if not TEST_IMAGE_PATH.exists():
    setup_test_image()
else:
    try:
        Image.open(TEST_IMAGE_PATH)
    except Exception:
        print(f"Test image at {TEST_IMAGE_PATH} seems invalid or missing, recreating.")
        setup_test_image()


def ensure_realcugan():
    # print("Attempting to ensure RealCUGAN...") # Keep minimal prints unless debugging
    dest = BIN_DIR / 'realcugan'
    archive_name = Path(REALCUGAN_URL).name
    extracted_root_dir_name = archive_name.replace('.zip', '')

    exe_abs_path = dest / Path(extracted_root_dir_name) / 'realcugan-ncnn-vulkan'
    # print(f"Expected RealCUGAN exe path: {exe_abs_path}")

    global REALCUGAN_AVAILABLE

    if not exe_abs_path.exists():
        print(f"RealCUGAN exe not found at {exe_abs_path}. Proceeding with download/extraction.")
        archive_abs_path = dest / archive_name
        download_archive(REALCUGAN_URL, archive_abs_path)
        if archive_abs_path.exists():
            print(f"Extracting {archive_abs_path} to {dest} (RealCUGAN)...")
            try:
                with zipfile.ZipFile(archive_abs_path, 'r') as z:
                    z.extractall(dest)
            except Exception as e:
                pytest.fail(f"Error extracting {archive_abs_path}: {e}")
        else:
            pytest.fail(f"Archive {archive_abs_path} failed to download.")

    if not exe_abs_path.exists():
        pytest.fail(f"RealCUGAN executable not found at {exe_abs_path} after supposed extraction.")

    if not os.access(exe_abs_path, os.X_OK):
        print(f"Making {exe_abs_path} executable...")
        exe_abs_path.chmod(0o755)
    if REALCUGAN_AVAILABLE is None:
        probe_out = dest / "probe.png"
        cmd_probe = [
            str(exe_abs_path),
            "-i",
            str(TEST_IMAGE_PATH),
            "-o",
            str(probe_out),
            "-s",
            "1",
            "-n",
            "1",
            "-m",
            str(exe_abs_path.parent / REALCUGAN_DEFAULT_MODEL_DIR_NAME),
        ]
        result = subprocess.run(cmd_probe, capture_output=True, text=True)
        if result.returncode != 0 or not probe_out.exists():
            print("RealCUGAN self-test failed, skipping binary tests.")
            print(f"stdout:\n{result.stdout}\nstderr:\n{result.stderr}")
            REALCUGAN_AVAILABLE = False
        else:
            probe_out.unlink(missing_ok=True)
            REALCUGAN_AVAILABLE = True
    if not REALCUGAN_AVAILABLE:
        pytest.skip("RealCUGAN binary not usable on this platform.")
    # print("RealCUGAN ensured successfully.")
    return exe_abs_path


def ensure_realesrgan():
    # print("Attempting to ensure RealESRGAN...") # Keep minimal prints
    global REALESRGAN_AVAILABLE
    dest = BIN_DIR / 'realesrgan'
    archive_name = Path(REALESRGAN_URL).name
    extracted_root_dir_name = archive_name.replace('.zip', '')

    exe_abs_path = dest / Path(extracted_root_dir_name) / 'realesrgan-ncnn-vulkan'
    # print(f"Expected RealESRGAN exe path: {exe_abs_path}")

    models_dir_abs = exe_abs_path.parent / 'models'

    if not exe_abs_path.exists():
        print(f"RealESRGAN exe not found at {exe_abs_path}. Proceeding with download/extraction.")
        archive_abs_path = dest / archive_name
        download_archive(REALESRGAN_URL, archive_abs_path)
        if archive_abs_path.exists():
            print(f"Extracting {archive_abs_path} to {dest} (RealESRGAN)...")
            try:
                with zipfile.ZipFile(archive_abs_path, 'r') as z:
                    z.extractall(dest)
            except Exception as e:
                pytest.fail(f"Error extracting {archive_abs_path} (RealESRGAN): {e}")
        else:
            pytest.fail(f"Archive {archive_abs_path} (RealESRGAN) failed to download.")

    if not exe_abs_path.exists():
        pytest.fail(f"RealESRGAN executable not found at {exe_abs_path} after supposed extraction.")

    if not os.access(exe_abs_path, os.X_OK):
        print(f"Making {exe_abs_path} executable (RealESRGAN)...")
        exe_abs_path.chmod(0o755)

    if not models_dir_abs.exists():
        print(f"Note: RealESRGAN models directory {models_dir_abs} was not found in the extracted archive. Tests requiring external models will skip.")
    if REALESRGAN_AVAILABLE is None:
        probe_out = dest / "probe_esrgan.png"
        cmd_probe = [
            str(exe_abs_path),
            "-i",
            str(TEST_IMAGE_PATH),
            "-o",
            str(probe_out),
            "-n",
            "realesrgan-x4plus",
            "-s",
            "2",
        ]
        result = subprocess.run(cmd_probe, capture_output=True, text=True)
        if result.returncode != 0 or not probe_out.exists():
            print("RealESRGAN self-test failed, skipping binary tests.")
            print(f"stdout:\n{result.stdout}\nstderr:\n{result.stderr}")
            REALESRGAN_AVAILABLE = False
        else:
            probe_out.unlink(missing_ok=True)
            REALESRGAN_AVAILABLE = True
    if not REALESRGAN_AVAILABLE:
        pytest.skip("RealESRGAN binary not usable on this platform.")
    # print("RealESRGAN ensured successfully.")
    return exe_abs_path

# --- RealCUGAN Tests ---
REALCUGAN_DEFAULT_MODEL_DIR_NAME = "models-se"
REALCUGAN_MODEL_FOLDERS = ["models-se", "models-nose", "models-pro"]
REALCUGAN_SCALES_MAP = {
    "models-se": [2, 4],
    "models-nose": [2, 4],
    "models-pro": [2, 3],
}
REALCUGAN_DENOISE_LEVELS = [-1, 0, 1, 2, 3]
REALCUGAN_TTA_OPTIONS = [False, True]

@pytest.mark.parametrize("model_folder_name", REALCUGAN_MODEL_FOLDERS)
@pytest.mark.parametrize("denoise_level", REALCUGAN_DENOISE_LEVELS)
@pytest.mark.parametrize("tta", REALCUGAN_TTA_OPTIONS)
def test_realcugan_detailed(tmp_path, model_folder_name, denoise_level, tta, subtests):
    exe = ensure_realcugan()
    input_img_abs = TEST_IMAGE_PATH

    if not input_img_abs.exists():
        pytest.skip(f"Input image not found: {input_img_abs}") # Should not happen if setup_test_image works

    model_dir_abs = exe.parent / model_folder_name # Models are in folders relative to exe's parent
    if not model_dir_abs.exists():
        pytest.skip(f"Model directory not found: {model_dir_abs} (for model set {model_folder_name})")

    scales_to_test = REALCUGAN_SCALES_MAP.get(model_folder_name, [2])

    for scale in scales_to_test:
        with subtests.test(msg=f"model_{model_folder_name}_scale_{scale}x_denoise_{denoise_level}_tta_{tta}"):
            output_img_filename = f"out_{model_folder_name}_s{scale}_n{denoise_level}_tta{int(tta)}.png"
            output_img_abs = tmp_path / output_img_filename

            cmd = [
                str(exe),
                '-i', str(input_img_abs),
                '-o', str(output_img_abs),
                '-s', str(scale),
                '-n', str(denoise_level),
                '-m', str(model_dir_abs)
            ]
            if tta:
                cmd.append('-x')

            print(f"Running RealCUGAN: {' '.join(cmd)}")
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=180) # Increased timeout
            if result.returncode != 0:
                print(f"RealCUGAN Stdout:\n{result.stdout}")
                print(f"RealCUGAN Stderr:\n{result.stderr}")
            assert result.returncode == 0, f"RealCUGAN failed"
            assert output_img_abs.exists(), f"Output image not created: {output_img_abs}"

            try:
                with Image.open(input_img_abs) as img_in, Image.open(output_img_abs) as img_out:
                    expected_width = img_in.width * scale
                    expected_height = img_in.height * scale
                    assert img_out.width == expected_width, f"Output width incorrect. Expected {expected_width}, got {img_out.width}"
                    assert img_out.height == expected_height, f"Output height incorrect. Expected {expected_height}, got {img_out.height}"
            except Exception as e:
                pytest.fail(f"Error during image dimension check or invalid image: {e}")


# --- RealESRGAN Tests ---
REALESRGAN_MODELS_SCALES = {
    "realesrgan-x4plus": 4,
    "realesrgan-x4plus-anime": 4,
    "realesrnet-x4plus": 4,
}
REALESRGAN_TTA_OPTIONS = [False, True]

@pytest.mark.parametrize("model_name", REALESRGAN_MODELS_SCALES.keys())
@pytest.mark.parametrize("tta", REALESRGAN_TTA_OPTIONS)
def test_realesrgan_detailed(tmp_path, model_name, tta, subtests):
    exe = ensure_realesrgan()
    input_img_abs = TEST_IMAGE_PATH

    if not input_img_abs.exists():
        pytest.skip(f"Input image not found: {input_img_abs}")

    model_param_abs = exe.parent / 'models' / f"{model_name}.param"
    model_bin_abs = exe.parent / 'models' / f"{model_name}.bin"

    if not model_param_abs.exists() or not model_bin_abs.exists():
         pytest.skip(f"Model files for {model_name} not found in {exe.parent / 'models'}. This is expected for this binary version.")

    scale = REALESRGAN_MODELS_SCALES[model_name]
    output_img_filename = f"out_{model_name}_tta{int(tta)}.png"
    output_img_abs = tmp_path / output_img_filename

    cmd = [
        str(exe),
        '-i', str(input_img_abs),
        '-o', str(output_img_abs),
        '-n', model_name,
        '-s', str(scale)
    ]
    if tta:
        cmd.append('-x')

    print(f"Running RealESRGAN: {' '.join(cmd)}")
    result = subprocess.run(cmd, capture_output=True, text=True, timeout=180) # Increased timeout
    if result.returncode != 0:
        print(f"RealESRGAN Stdout:\n{result.stdout}")
        print(f"RealESRGAN Stderr:\n{result.stderr}")

    assert result.returncode == 0, f"RealESRGAN failed"
    assert output_img_abs.exists(), f"Output image not created: {output_img_abs}"

    try:
        with Image.open(input_img_abs) as img_in, Image.open(output_img_abs) as img_out:
            expected_width = img_in.width * scale
            expected_height = img_in.height * scale
            assert img_out.width == expected_width, f"Output width incorrect. Expected {expected_width}, got {img_out.width}"
            assert img_out.height == expected_height, f"Output height incorrect. Expected {expected_height}, got {img_out.height}"
    except Exception as e:
        pytest.fail(f"Error during image dimension check or invalid image: {e}")


def test_realcugan_original_basic(tmp_path):
    exe = ensure_realcugan()
    input_img = TEST_IMAGE_PATH # Use common test image

    if not input_img.exists():
        pytest.skip(f"Input image {input_img} not found for basic RealCUGAN test.")
    output_img = tmp_path / 'out_basic_cugan.png'

    # Models are in folders like "models-se" inside the extracted directory (exe.parent)
    model_dir_abs = exe.parent / REALCUGAN_DEFAULT_MODEL_DIR_NAME

    # For basic test, use a specific 2x model from the default set
    basic_test_model_name = "up2x-conservative"
    # The -m argument for realcugan is the FOLDER containing the model files.
    # The specific model (param/bin) is usually inferred or hardcoded in the binary based on scale/denoise.
    # However, some versions might allow specifying model files directly.
    # For this binary, it expects model directory.

    model_param_file = model_dir_abs / f"{basic_test_model_name}.param"
    if not model_param_file.exists():
         pytest.skip(f"Basic test model file {model_param_file} not found in {model_dir_abs}")

    cmd_list = [str(exe), '-i', str(input_img), '-o', str(output_img), '-s', '2', '-n', '1', '-m', str(model_dir_abs)]
    print(f"Running RealCUGAN basic command: {' '.join(cmd_list)}")
    result = subprocess.run(cmd_list, capture_output=True, text=True, timeout=60)
    print(f"RealCUGAN basic stdout:\n{result.stdout}")
    print(f"RealCUGAN basic stderr:\n{result.stderr}")
    assert result.returncode == 0, result.stderr
    assert output_img.exists()

def test_realesrgan_original_basic(tmp_path):
    exe = ensure_realesrgan()
    input_img = TEST_IMAGE_PATH # Use common test image

    if not input_img.exists():
        pytest.skip(f"Input image {input_img} not found for basic RealESRGAN test.")
    output_img = tmp_path / 'out_basic_esrgan.png'
    default_model_name = "realesrgan-x4plus"
    default_scale = REALESRGAN_MODELS_SCALES.get(default_model_name, 4)

    models_dir = exe.parent / 'models'
    model_param_abs = models_dir / f"{default_model_name}.param"
    model_bin_abs = models_dir / f"{default_model_name}.bin"

    if not models_dir.exists() or not model_param_abs.exists() or not model_bin_abs.exists():
         pytest.skip(f"Default model files for {default_model_name} not found in {models_dir}. This is expected for this binary version.")

    cmd_list = [str(exe), '-i', str(input_img), '-o', str(output_img), '-n', default_model_name, '-s', str(default_scale)]
    print(f"Running RealESRGAN basic command: {' '.join(cmd_list)}")
    result = subprocess.run(cmd_list, capture_output=True, text=True, timeout=60)
    print(f"RealESRGAN basic stdout:\n{result.stdout}")
    print(f"RealESRGAN basic stderr:\n{result.stderr}")
    assert result.returncode == 0, result.stderr
    assert output_img.exists()
