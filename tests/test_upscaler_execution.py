import subprocess
import os
import sys # For platform check
import pytest
from pathlib import Path
from PIL import Image

# Define base directories
REPO_ROOT = Path(__file__).parent.parent # Assuming tests/ is one level down from repo root
APP_DIR_NAME = "Waifu2x-Extension-QT"
CI_BIN_DIR_NAME = "bin"
TESTS_DIR = Path(__file__).parent

# Define the names of the upscaler executables based on platform.
if sys.platform == "win32":
    RCUGAN_EXE_NAME = "realcugan-ncnn-vulkan.exe"
    RESRGAN_EXE_NAME = "realesrgan-ncnn-vulkan.exe"
else: # linux, darwin, etc.
    RCUGAN_EXE_NAME = "realcugan-ncnn-vulkan"
    RESRGAN_EXE_NAME = "realesrgan-ncnn-vulkan"

def find_executable_path(exe_name):
    """Checks CI bin path first, then standard app path."""
    # Path for CI environment (e.g., tests/bin/executable)
    ci_path = TESTS_DIR / CI_BIN_DIR_NAME / exe_name
    if ci_path.is_file():
        return str(ci_path)

    # Path for local build environment (e.g., Waifu2x-Extension-QT/executable)
    app_path = REPO_ROOT / APP_DIR_NAME / exe_name
    if app_path.is_file():
        return str(app_path)

    # Path for local build environment (e.g., Waifu2x-Extension-QT/Engines/engine_dir/executable)
    # This matches the C++ code's assumption for Current_Path + "/Engines/..."
    # RealCUGAN
    if exe_name == RCUGAN_EXE_NAME:
        app_engine_path = REPO_ROOT / APP_DIR_NAME / "Engines" / "realcugan-ncnn-vulkan" / exe_name
        if app_engine_path.is_file():
            return str(app_engine_path)
    # RealESRGAN
    if exe_name == RESRGAN_EXE_NAME:
        app_engine_path = REPO_ROOT / APP_DIR_NAME / "Engines" / "realesrgan-ncnn-vulkan" / exe_name
        if app_engine_path.is_file():
            return str(app_engine_path)

    return None # Executable not found

# Path to the small test image (created by test_binaries.py in tests/)
TEST_IMAGE_FILENAME = "test_image.png"
TEST_IMAGE_PATH_REL_TO_TESTS = TESTS_DIR / TEST_IMAGE_FILENAME

# Timeout for subprocess execution
TIMEOUT_SECONDS = 30 # Increased timeout for upscaling

def get_model_dir_path(engine_name, model_subdir):
    """Returns the most likely path for models, preferring APP_DIR/Engines structure."""
    # Path for local build environment (e.g., Waifu2x-Extension-QT/Engines/engine_dir/model_subdir)
    app_engine_models_path = REPO_ROOT / APP_DIR_NAME / "Engines" / engine_name / model_subdir
    if app_engine_models_path.is_dir():
        return str(app_engine_models_path)

    # Fallback to APP_DIR/model_subdir (older structure or if engines are directly in APP_DIR)
    app_models_path = REPO_ROOT / APP_DIR_NAME / model_subdir
    if app_models_path.is_dir():
        return str(app_models_path)

    # Fallback to tests/bin/model_subdir (CI structure)
    ci_models_path = TESTS_DIR / CI_BIN_DIR_NAME / model_subdir
    if ci_models_path.is_dir():
        return str(ci_models_path)

    return None


def test_realcugan_execution_and_basic_upscale(tmp_path):
    """
    Tests if the RealCUGAN executable exists, runs, and performs a basic upscale.
    """
    exe_path = find_executable_path(RCUGAN_EXE_NAME)
    if not exe_path:
        pytest.skip(f"{RCUGAN_EXE_NAME} not found in standard locations.")

    input_image_path = TEST_IMAGE_PATH_REL_TO_TESTS
    output_image_path = tmp_path / "test_realcugan_out.png"

    # RealCUGAN models are specified by a directory path.
    model_dir_path = get_model_dir_path("realcugan-ncnn-vulkan", "models-se")
    if not model_dir_path:
        pytest.skip(f"RealCUGAN model directory 'models-se' not found.")

    print(f"Using RealCUGAN executable at: {os.path.abspath(exe_path)}")
    print(f"Using RealCUGAN model directory at: {os.path.abspath(model_dir_path)}")

    if not input_image_path.exists():
        pytest.skip(f"Test input image {input_image_path} not found.")

    # Basic device check (optional, but good for quick failure)
    cmd_list_devices = [exe_path]
    result_devices = subprocess.run(cmd_list_devices, capture_output=True, text=True, timeout=TIMEOUT_SECONDS, check=False)
    assert result_devices.returncode == 0, f"{RCUGAN_EXE_NAME} (device check) exited with {result_devices.returncode}. Stderr: '{result_devices.stderr}'. Stdout: '{result_devices.stdout}'"
    assert "device" in (result_devices.stdout.lower() + result_devices.stderr.lower()), f"{RCUGAN_EXE_NAME} (device check) did not print device info."

    # Perform upscaling
    cmd_upscale = [
        exe_path,
        "-i", str(input_image_path),
        "-o", str(output_image_path),
        "-s", "2", # Scale
        "-n", "0", # Denoise level: 0 for this model type (conservative)
        "-m", model_dir_path # Path to model directory
    ]
    print(f"Running RealCUGAN upscale: {' '.join(cmd_upscale)}")
    result_upscale = subprocess.run(cmd_upscale, capture_output=True, text=True, timeout=TIMEOUT_SECONDS, check=False)

    assert result_upscale.returncode == 0, f"RealCUGAN upscale failed with code {result_upscale.returncode}. Stderr: '{result_upscale.stderr}'. Stdout: '{result_upscale.stdout}'"
    assert output_image_path.exists(), f"RealCUGAN output image {output_image_path} not created."

    try:
        with Image.open(input_image_path) as img_in, Image.open(output_image_path) as img_out:
            expected_width = img_in.width * 2
            expected_height = img_in.height * 2
            assert img_out.width == expected_width, f"Output width incorrect. Expected {expected_width}, got {img_out.width}"
            assert img_out.height == expected_height, f"Output height incorrect. Expected {expected_height}, got {img_out.height}"
    except Exception as e:
        pytest.fail(f"Error opening images or checking dimensions: {e}")

def test_realesrgan_execution_and_basic_upscale(tmp_path):
    """
    Tests if the RealESRGAN executable exists, runs, and performs a basic upscale.
    """
    exe_path = find_executable_path(RESRGAN_EXE_NAME)
    if not exe_path:
        pytest.skip(f"{RESRGAN_EXE_NAME} not found in standard locations.")

    input_image_path = TEST_IMAGE_PATH_REL_TO_TESTS
    output_image_path = tmp_path / "test_realesrgan_out.png"

    model_name = "realesr-animevideov3-x2"
    expected_scale = 2

    # RealESRGAN models are specified by a model name, and the -m flag points to the *directory containing these named model files*.
    # The executable usually prepends this path to the model name.
    # e.g. -m models -n realesr-animevideov3-x2 -> looks for models/realesr-animevideov3-x2.param
    models_dir_param = get_model_dir_path("realesrgan-ncnn-vulkan", "models")
    if not models_dir_param:
         pytest.skip(f"RealESRGAN models directory 'models' not found.")

    print(f"Using RealESRGAN executable at: {os.path.abspath(exe_path)}")
    print(f"Using RealESRGAN models directory parameter: {os.path.abspath(models_dir_param)}")

    # Specific model file check (optional, but good for debugging)
    if not (Path(models_dir_param) / (model_name + ".param")).is_file():
         pytest.skip(f"RealESRGAN model {model_name}.param not found in {os.path.abspath(models_dir_param)}")
    if not (Path(models_dir_param) / (model_name + ".bin")).is_file():
         pytest.skip(f"RealESRGAN model {model_name}.bin not found in {os.path.abspath(models_dir_param)}")

    if not input_image_path.exists():
        pytest.skip(f"Test input image {input_image_path} not found.")

    # Basic device check
    cmd_list_devices = [exe_path]
    result_devices = subprocess.run(cmd_list_devices, capture_output=True, text=True, timeout=TIMEOUT_SECONDS, check=False)
    assert result_devices.returncode == 0, f"{RESRGAN_EXE_NAME} (device check) exited with {result_devices.returncode}. Stderr: '{result_devices.stderr}'. Stdout: '{result_devices.stdout}'"
    assert "device" in (result_devices.stdout.lower() + result_devices.stderr.lower()), f"{RESRGAN_EXE_NAME} (device check) did not print device info."

    # Perform upscaling
    cmd_upscale = [
        exe_path,
        "-i", str(input_image_path),
        "-o", str(output_image_path),
        "-s", str(expected_scale),
        "-n", model_name,
        "-m", models_dir_param # Pass the models directory
    ]
    # Set working directory to where the executable is, so it can find its relative model paths if not using absolute for -m
    executable_dir = Path(exe_path).parent
    print(f"Running RealESRGAN upscale: {' '.join(cmd_upscale)} in CWD: {executable_dir}")
    result_upscale = subprocess.run(cmd_upscale, capture_output=True, text=True, timeout=TIMEOUT_SECONDS, check=False, cwd=executable_dir)

    assert result_upscale.returncode == 0, f"RealESRGAN upscale failed with code {result_upscale.returncode}. Stderr: '{result_upscale.stderr}'. Stdout: '{result_upscale.stdout}'"
    assert output_image_path.exists(), f"RealESRGAN output image {output_image_path} not created."

    try:
        with Image.open(input_image_path) as img_in, Image.open(output_image_path) as img_out:
            expected_width = img_in.width * expected_scale
            expected_height = img_in.height * expected_scale
            assert img_out.width == expected_width, f"Output width incorrect. Expected {expected_width}, got {img_out.width}"
            assert img_out.height == expected_height, f"Output height incorrect. Expected {expected_height}, got {img_out.height}"
    except Exception as e:
        pytest.fail(f"Error opening images or checking dimensions: {e}")

def test_realesrgan_x4plus_anime_execution(tmp_path):
    """
    Tests RealESRGAN with the 'realesrgan-x4plus-anime' model at 4x scale.
    """
    exe_path = find_executable_path(RESRGAN_EXE_NAME)
    if not exe_path:
        pytest.skip(f"{RESRGAN_EXE_NAME} not found in standard locations.")

    input_image_path = TEST_IMAGE_PATH_REL_TO_TESTS
    output_image_path = tmp_path / "test_realesrgan_x4plus_anime_out.png"

    model_name = "realesrgan-x4plus-anime"
    expected_scale = 4

    models_dir_param = get_model_dir_path("realesrgan-ncnn-vulkan", "models")
    if not models_dir_param:
         pytest.skip(f"RealESRGAN models directory 'models' not found.")

    print(f"Using RealESRGAN executable at: {os.path.abspath(exe_path)}")
    print(f"Using RealESRGAN models directory parameter: {os.path.abspath(models_dir_param)}")

    if not (Path(models_dir_param) / (model_name + ".param")).is_file():
         pytest.skip(f"RealESRGAN model {model_name}.param not found in {os.path.abspath(models_dir_param)}")
    if not (Path(models_dir_param) / (model_name + ".bin")).is_file():
         pytest.skip(f"RealESRGAN model {model_name}.bin not found in {os.path.abspath(models_dir_param)}")

    if not input_image_path.exists():
        pytest.skip(f"Test input image {input_image_path} not found.")

    # Perform upscaling
    cmd_upscale = [
        exe_path,
        "-i", str(input_image_path),
        "-o", str(output_image_path),
        "-s", str(expected_scale), # Explicitly set scale, though model name implies it
        "-n", model_name,
        "-m", models_dir_param
    ]
    executable_dir = Path(exe_path).parent
    print(f"Running RealESRGAN (x4plus-anime) upscale: {' '.join(cmd_upscale)} in CWD: {executable_dir}")
    result_upscale = subprocess.run(cmd_upscale, capture_output=True, text=True, timeout=TIMEOUT_SECONDS, check=False, cwd=executable_dir)

    assert result_upscale.returncode == 0, f"RealESRGAN (x4plus-anime) upscale failed with code {result_upscale.returncode}. Stderr: '{result_upscale.stderr}'. Stdout: '{result_upscale.stdout}'"
    assert output_image_path.exists(), f"RealESRGAN (x4plus-anime) output image {output_image_path} not created."

    try:
        with Image.open(input_image_path) as img_in, Image.open(output_image_path) as img_out:
            expected_width = img_in.width * expected_scale
            expected_height = img_in.height * expected_scale
            assert img_out.width == expected_width, f"Output width incorrect. Expected {expected_width}, got {img_out.width}"
            assert img_out.height == expected_height, f"Output height incorrect. Expected {expected_height}, got {img_out.height}"
    except Exception as e:
        pytest.fail(f"Error opening images or checking dimensions: {e}")

if __name__ == "__main__":
    # This allows running the test directly using "python tests/test_upscaler_execution.py"
    # For pytest, it will discover the test_ functions automatically.
    # Note: For direct execution, tmp_path fixture won't be available.
    # A more complex setup or conditional logic would be needed for tmp_path if direct run is critical.
    # For now, this main block is simplified and primarily for pytest discovery context.

    print(f"Upscaler tests running. Expecting executables in: {os.path.abspath(APP_DIR)}")
    print(f"Platform: {sys.platform}")
    print(f"RealCUGAN executable: {RCUGAN_EXE_NAME}")
    print(f"RealESRGAN executable: {RESRGAN_EXE_NAME}")
    print(f"Test image path: {TEST_IMAGE_PATH_REL_TO_TESTS}")


    # Check if APP_DIR exists first, to give a clearer error if it doesn't.
    if not os.path.isdir(APP_DIR):
        print(f"ERROR: Application directory '{APP_DIR}' not found relative to current path '{os.getcwd()}'.")
        print("Please ensure the tests are run from the repository root, and the application has been built/executables placed.")
        sys.exit(1)

    if not TEST_IMAGE_PATH_REL_TO_TESTS.exists():
        print(f"ERROR: Test image '{TEST_IMAGE_PATH_REL_TO_TESTS}' not found.")
        print("Ensure 'test_image.png' is present in the 'tests' directory. It can be created by test_binaries.py.")
        # As a fallback for direct execution, let's try to create a dummy one if Pillow is available
        try:
            img = Image.new('RGB', (60, 30), color = 'red')
            img.save(TEST_IMAGE_PATH_REL_TO_TESTS)
            print(f"Created a dummy {TEST_IMAGE_FILENAME} for direct script run.")
        except Exception as e:
            print(f"Could not create dummy test image: {e}")
            sys.exit(1)

    # Note: The pytest tmp_path fixture is not available when running the script directly.
    # These direct calls will fail if they strictly rely on tmp_path.
    # This __main__ block is more for sanity checking paths and basic info.
    # Actual test execution with tmp_path should be done via `pytest`.
    print("\nTo run these tests properly, use 'pytest tests/test_upscaler_execution.py'")
    print("The following direct calls are for basic checks and will likely fail due to tmp_path.")

    # Simplified direct call for basic syntax check, will fail on tmp_path
    # print("\nAttempting direct call to test_realcugan_execution_and_basic_upscale (will fail on tmp_path)...")
    # try:
    #     # Create a dummy tmp_path for direct execution attempt
    #     from tempfile import TemporaryDirectory
    #     with TemporaryDirectory() as tmpdirname:
    #         test_realcugan_execution_and_basic_upscale(Path(tmpdirname))
    #     print("Direct call placeholder finished.")
    # except Exception as e:
    #     print(f"Direct call placeholder ERRORED: {e}")

    # print("\nAttempting direct call to test_realesrgan_execution_and_basic_upscale (will fail on tmp_path)...")
    # try:
    #     from tempfile import TemporaryDirectory
    #     with TemporaryDirectory() as tmpdirname:
    #         test_realesrgan_execution_and_basic_upscale(Path(tmpdirname))
    #     print("Direct call placeholder finished.")
    # except Exception as e:
    #     print(f"Direct call placeholder ERRORED: {e}")

    print("\nScript finished. Run with pytest for full test execution.")
