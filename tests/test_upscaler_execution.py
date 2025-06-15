import subprocess
import os
import sys # For platform check
import pytest
from pathlib import Path
from PIL import Image

# Define the expected directory where the main application and upscaler binaries are located.
# This should be relative to the repository root where pytest is typically run.
APP_DIR = "Waifu2x-Extension-QT"

# Define the names of the upscaler executables based on platform.
if sys.platform == "win32":
    RCUGAN_EXE_NAME = "realcugan-ncnn-vulkan.exe"
    RESRGAN_EXE_NAME = "realesrgan-ncnn-vulkan.exe"
else: # linux, darwin, etc.
    RCUGAN_EXE_NAME = "realcugan-ncnn-vulkan"
    RESRGAN_EXE_NAME = "realesrgan-ncnn-vulkan"

# Path to the small test image (created by test_binaries.py in tests/)
TEST_IMAGE_FILENAME = "test_image.png"
TEST_IMAGE_PATH_REL_TO_TESTS = Path(__file__).parent / TEST_IMAGE_FILENAME

# Timeout for subprocess execution
TIMEOUT_SECONDS = 30 # Increased timeout for upscaling

def test_realcugan_execution_and_basic_upscale(tmp_path):
    """
    Tests if the RealCUGAN executable exists, runs, and performs a basic upscale.
    """
    exe_path = os.path.join(APP_DIR, RCUGAN_EXE_NAME)
    input_image_path = TEST_IMAGE_PATH_REL_TO_TESTS
    output_image_path = tmp_path / "test_realcugan_out.png"
    # RealCUGAN models are specified by a directory path.
    # The build_projects.sh script should place these inside APP_DIR.
    model_dir_path = os.path.join(APP_DIR, "models-se")

    print(f"Checking for RealCUGAN at: {os.path.abspath(exe_path)}")
    if not os.path.isfile(exe_path):
        pytest.skip(f"{RCUGAN_EXE_NAME} not found at {os.path.abspath(exe_path)}")

    if not input_image_path.exists():
        pytest.skip(f"Test input image {input_image_path} not found.")

    if not os.path.isdir(model_dir_path):
        pytest.skip(f"RealCUGAN model directory not found at {os.path.abspath(model_dir_path)}")

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
    exe_path = os.path.join(APP_DIR, RESRGAN_EXE_NAME)
    input_image_path = TEST_IMAGE_PATH_REL_TO_TESTS
    output_image_path = tmp_path / "test_realesrgan_out.png"
    # RealESRGAN models are typically in a 'models' subdirectory relative to the exe.
    # The -n parameter specifies the model name (without .bin/.param extension).
    model_name = "realesr-animevideov3-x2" # A common 2x model
    expected_scale = 2

    models_base_dir = os.path.join(APP_DIR, "models") # Base directory where models like 'realesr-animevideov3-x2.bin' should be
    if not os.path.isdir(models_base_dir):
        pytest.skip(f"RealESRGAN base models directory not found at {os.path.abspath(models_base_dir)}")

    # Specific model file check (optional, as the exe will error out if not found)
    # For RealESRGAN, models are usually in APP_DIR/models/
    # The build_projects.sh copies them there.
    if not os.path.isfile(os.path.join(models_base_dir, model_name + ".param")):
         pytest.skip(f"RealESRGAN model {model_name}.param not found in {os.path.abspath(models_base_dir)}")
    if not os.path.isfile(os.path.join(models_base_dir, model_name + ".bin")):
         pytest.skip(f"RealESRGAN model {model_name}.bin not found in {os.path.abspath(models_base_dir)}")


    print(f"Checking for RealESRGAN at: {os.path.abspath(exe_path)}")
    if not os.path.isfile(exe_path):
        pytest.skip(f"{RESRGAN_EXE_NAME} not found at {os.path.abspath(exe_path)}")

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
        "-n", model_name # Model name, exe prepends "models/" path
    ]
    print(f"Running RealESRGAN upscale: {' '.join(cmd_upscale)}")
    result_upscale = subprocess.run(cmd_upscale, capture_output=True, text=True, timeout=TIMEOUT_SECONDS, check=False)

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
