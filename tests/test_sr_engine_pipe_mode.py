import subprocess
import os
import sys
import pytest
from pathlib import Path
import shutil

# Define base directories
REPO_ROOT = Path(__file__).parent.parent
TESTS_DIR = Path(__file__).parent
CI_BIN_DIR_NAME = "bin" # In case executables are copied into tests/bin for CI

# SR Engine executable base names (platform-specific extension handled in find_sr_executable_path)
# These are the names used by the C++ code when looking for models etc.
RCUGAN_EXE_BASE_NAME = "realcugan-ncnn-vulkan"
RESRGAN_EXE_BASE_NAME = "realesrgan-ncnn-vulkan"

def find_sr_executable_path(exe_name_base):
    """
    Checks common locations for SR engine executables.
    Tries exe_name_base, exe_name_base.exe, and common short names like realcugan.exe.
    """

    names_to_try = [exe_name_base]
    if not exe_name_base.endswith('.exe'):
        names_to_try.append(exe_name_base + ".exe")

    # Add common short names found in Waifu2x-Extension-QT-Launcher
    if exe_name_base == RCUGAN_EXE_BASE_NAME:
        if "realcugan.exe" not in names_to_try: names_to_try.append("realcugan.exe")
        if "realcugan" not in names_to_try: names_to_try.append("realcugan") # For non-windows short name
    elif exe_name_base == RESRGAN_EXE_BASE_NAME:
        if "realesrgan.exe" not in names_to_try: names_to_try.append("realesrgan.exe")
        if "realesrgan" not in names_to_try: names_to_try.append("realesrgan")

    # Deduplicate while preserving order somewhat (specific names first)
    final_names_to_try = []
    for n in names_to_try:
        if n not in final_names_to_try:
            final_names_to_try.append(n)

    # Define search directory patterns
    # engine_dir_name is derived from the *base* name, e.g., "realcugan-ncnn-vulkan"
    engine_dir_name = exe_name_base.split('-ncnn-vulkan')[0] + "-ncnn-vulkan"

    # Order of preference for directories:
    # 1. App's Engines dir (primary target for app)
    # 2. Engine's own repo (root and typical 'windows' or 'build' subdirs)
    # 3. Launcher directory (often has prebuilt binaries)
    # 4. CI bin directory
    # 5. shutil.which (PATH)

    search_dirs = [
        REPO_ROOT / "Waifu2x-Extension-QT" / "Engines" / engine_dir_name,
        REPO_ROOT / "Waifu2x-Extension-QT" / "Engines" / engine_dir_name / "windows",
        REPO_ROOT / engine_dir_name,
        REPO_ROOT / engine_dir_name / "windows",
        REPO_ROOT / engine_dir_name / "build", # Common build output folder
        REPO_ROOT / engine_dir_name / "src",   # Less likely, but possible
        REPO_ROOT / "Waifu2x-Extension-QT-Launcher",
        TESTS_DIR / CI_BIN_DIR_NAME,
    ]

    for exe_variant_name in final_names_to_try:
        for loc in search_dirs:
            potential_path = (loc / exe_variant_name).resolve()
            if potential_path.is_file() and os.access(potential_path, os.X_OK):
                print(f"Found executable: {potential_path}")
                return str(potential_path)

        # Fallback: check PATH environment variable for this variant
        exe_on_path = shutil.which(exe_variant_name)
        if exe_on_path:
            print(f"Found executable via PATH: {exe_on_path}")
            return str(exe_on_path)

    print(f"Executable based on '{exe_name_base}' (tried variants: {final_names_to_try}) not found in any checked locations or PATH.")
    return None


TIMEOUT_SECONDS = 30

REALCUGAN_MODEL_PATH_STEM_FOR_TEST = "models-se/up2x-no-denoise" # Relative to engine's models directory
REALESRGAN_MODELS_DIR_NAME_FOR_TESTS = "models" # The actual folder name containing .param/.bin
REALESRGAN_MODEL_NAME_STEM_FOR_TEST = "realesr-animevideov3-x2"


def get_model_path_for_realcugan(exe_path_str, model_stem_rel_to_engine_root_models_dir):
    """
    For RealCUGAN, -m expects the path to model files without extension.
    Searches relative to exe, then in standard engine model dirs.
    e.g., model_stem_rel_to_engine_models_dir = "models-se/up2x-no-denoise"
    """
    if not exe_path_str: return None
    exe_p = Path(exe_path_str)

    # Determine engine directory name from executable name
    engine_repo_dir_name = exe_p.name.replace(".exe", "").split('-ncnn-vulkan')[0] + "-ncnn-vulkan"

    # Search bases, trying to guess where the 'models' or 'models-se' directory might be
    # relative to standard locations of the engine's repo or where it's deployed by the app.
    search_bases = [
        exe_p.parent, # Relative to executable itself first
        exe_p.parent / "..", # One level up from exe (e.g. if exe is in 'windows' or 'Engines/<name>')
        REPO_ROOT / engine_repo_dir_name, # Root of the engine's own repo
        REPO_ROOT / "Waifu2x-Extension-QT" / "Engines" / engine_repo_dir_name, # App deployment structure
        REPO_ROOT / "Waifu2x-Extension-QT" # If models are flat in app dir (less likely for these)
    ]

    for base_dir in search_bases:
        # model_stem_rel_to_engine_models_dir already includes "models-se/" part.
        potential_model_path = (base_dir / model_stem_rel_to_engine_models_dir).resolve()
        if potential_model_path.with_suffix(".param").exists() and \
           potential_model_path.with_suffix(".bin").exists():
            print(f"Found RealCUGAN model stem at: {potential_model_path}")
            return str(potential_model_path)

    print(f"RealCUGAN model stem '{model_stem_rel_to_engine_models_dir}' not found from base {exe_p.parent} or std locations.")
    return None

def get_models_dir_for_realesrgan(exe_path_str, models_subdir_name_in_engine):
    """
    For RealESRGAN, -m expects the path to the directory containing model files (e.g., "models").
    Searches relative to exe, then in standard engine model dirs.
    """
    if not exe_path_str: return None
    exe_p = Path(exe_path_str)
    engine_repo_dir_name = exe_p.name.replace(".exe", "").split('-ncnn-vulkan')[0] + "-ncnn-vulkan"

    search_bases = [
        exe_p.parent,
        exe_p.parent / "..",
        REPO_ROOT / engine_repo_dir_name,
        REPO_ROOT / "Waifu2x-Extension-QT" / "Engines" / engine_repo_dir_name,
        REPO_ROOT / "Waifu2x-Extension-QT"
    ]

    for base_dir in search_bases:
        potential_models_dir = (base_dir / models_subdir_name_in_engine).resolve()
        if potential_models_dir.is_dir():
            # Check if it actually contains some models to be reasonably sure
            if list(potential_models_dir.glob(f"{REALESRGAN_MODEL_NAME_STEM_FOR_TEST}.param")):
                 print(f"Found RealESRGAN models directory at: {potential_models_dir}")
                 return str(potential_models_dir)

    print(f"RealESRGAN models directory '{models_subdir_name_in_engine}' not found from base {exe_p.parent} or std locations.")
    return None

# Use the base names for skipif conditions, find_sr_executable_path will handle variants
@pytest.mark.skipif(not find_sr_executable_path(RCUGAN_EXE_BASE_NAME), reason=f"{RCUGAN_EXE_BASE_NAME} not found or not executable.")
def test_realcugan_pipe_mode_success():
    exe_path = find_sr_executable_path(RCUGAN_EXE_BASE_NAME)
    model_path_arg = get_model_path_for_realcugan(exe_path, REALCUGAN_MODEL_PATH_STEM_FOR_TEST)
    if not model_path_arg:
        pytest.skip(f"RealCUGAN model stem '{REALCUGAN_MODEL_PATH_STEM_FOR_TEST}' not found for exe {exe_path}.")

    input_w, input_h, input_channels, scale = 2, 2, 3, 2
    raw_bgr_frame = bytes([0,0,255, 0,255,0, 255,0,0, 255,255,255])

    cmd = [
        exe_path, "--use-pipe", "--verbose", # Added verbose for more debug
        "--input-width", str(input_w), "--input-height", str(input_h),
        "--input-channels", str(input_channels), "--pixel-format", "BGR",
        "-s", str(scale), "-m", model_path_arg, "-n", "0"
    ]
    process = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout_data, stderr_data = process.communicate(input=raw_bgr_frame, timeout=TIMEOUT_SECONDS)

    stderr_decoded = stderr_data.decode(errors='ignore')
    print(f"RealCUGAN Success Test: CMD: {' '.join(cmd)}")
    print(f"RealCUGAN Success Test: Stderr: {stderr_decoded}")
    assert process.returncode == 0, f"RealCUGAN pipe failed. stderr: {stderr_decoded}"

    expected_bytes = (input_w * scale) * (input_h * scale) * input_channels
    assert len(stdout_data) == expected_bytes, f"RealCUGAN output data size. Expected {expected_bytes}, got {len(stdout_data)}"

@pytest.mark.skipif(not find_sr_executable_path(RCUGAN_EXE_BASE_NAME), reason=f"{RCUGAN_EXE_BASE_NAME} not found or not executable.")
def test_realcugan_pipe_mode_insufficient_stdin():
    exe_path = find_sr_executable_path(RCUGAN_EXE_BASE_NAME)
    model_path_arg = get_model_path_for_realcugan(exe_path, REALCUGAN_MODEL_PATH_STEM_FOR_TEST)
    if not model_path_arg:
        pytest.skip(f"RealCUGAN model stem '{REALCUGAN_MODEL_PATH_STEM_FOR_TEST}' not found for exe {exe_path}.")

    input_w, input_h, input_channels, scale = 2, 2, 3, 2
    raw_bgr_frame_incomplete = bytes([0,0,255, 0,255,0])
    cmd = [
        exe_path, "--use-pipe", "--verbose",
        "--input-width", str(input_w), "--input-height", str(input_h),
        "--input-channels", str(input_channels), "--pixel-format", "BGR",
        "-s", str(scale), "-m", model_path_arg, "-n", "0"
    ]
    process = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    _, stderr_data = process.communicate(input=raw_bgr_frame_incomplete, timeout=TIMEOUT_SECONDS)

    stderr_decoded = stderr_data.decode(errors='ignore')
    print(f"RealCUGAN Insufficient Stdin Test: CMD: {' '.join(cmd)}")
    print(f"RealCUGAN Insufficient Stdin Test: Stderr: {stderr_decoded}")
    assert process.returncode != 0, "RealCUGAN should have failed (insufficient stdin) but exited with 0."
    assert "could not read full frame from stdin" in stderr_decoded.lower()

@pytest.mark.skipif(not find_sr_executable_path(RESRGAN_EXE_BASE_NAME), reason=f"{RESRGAN_EXE_BASE_NAME} not found or not executable.")
def test_realesrgan_pipe_mode_success():
    exe_path = find_sr_executable_path(RESRGAN_EXE_BASE_NAME)
    models_dir_arg = get_models_dir_for_realesrgan(exe_path, REALESRGAN_MODELS_DIR_NAME_FOR_TESTS)
    if not models_dir_arg:
        pytest.skip(f"RealESRGAN models dir '{REALESRGAN_MODELS_DIR_NAME_FOR_TESTS}' not found for exe {exe_path}.")
    if not (Path(models_dir_arg) / (REALESRGAN_MODEL_NAME_STEM_FOR_TEST + ".param")).exists():
        pytest.skip(f"RealESRGAN model '{REALESRGAN_MODEL_NAME_STEM_FOR_TEST}' not found in {models_dir_arg}.")

    input_w, input_h, input_channels, scale = 2, 2, 3, 2
    raw_bgr_frame = bytes([0,0,255, 0,255,0, 255,0,0, 255,255,255])

    cmd = [
        exe_path, "--use-pipe", "--verbose",
        "--input-width", str(input_w), "--input-height", str(input_h),
        "--input-channels", str(input_channels), "--pixel-format", "BGR",
        "-s", str(scale), "-m", models_dir_arg, "-n", REALESRGAN_MODEL_NAME_STEM_FOR_TEST
    ]
    process = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout_data, stderr_data = process.communicate(input=raw_bgr_frame, timeout=TIMEOUT_SECONDS)

    stderr_decoded = stderr_data.decode(errors='ignore')
    print(f"RealESRGAN Success Test: CMD: {' '.join(cmd)}")
    print(f"RealESRGAN Success Test: Stderr: {stderr_decoded}")
    assert process.returncode == 0, f"RealESRGAN pipe failed. stderr: {stderr_decoded}"

    expected_bytes = (input_w * scale) * (input_h * scale) * input_channels
    assert len(stdout_data) == expected_bytes, f"RealESRGAN output data size. Expected {expected_bytes}, got {len(stdout_data)}"

@pytest.mark.skipif(not find_sr_executable_path(RESRGAN_EXE_BASE_NAME), reason=f"{RESRGAN_EXE_BASE_NAME} not found or not executable.")
def test_realesrgan_pipe_mode_insufficient_stdin():
    exe_path = find_sr_executable_path(RESRGAN_EXE_BASE_NAME)
    models_dir_arg = get_models_dir_for_realesrgan(exe_path, REALESRGAN_MODELS_DIR_NAME_FOR_TESTS)
    if not models_dir_arg:
        pytest.skip(f"RealESRGAN models dir '{REALESRGAN_MODELS_DIR_NAME_FOR_TESTS}' not found for exe {exe_path}.")
    if not (Path(models_dir_arg) / (REALESRGAN_MODEL_NAME_STEM_FOR_TEST + ".param")).exists():
        pytest.skip(f"RealESRGAN model '{REALESRGAN_MODEL_NAME_STEM_FOR_TEST}' not found in {models_dir_arg}.")

    input_w, input_h, input_channels, scale = 2, 2, 3, 2
    raw_bgr_frame_incomplete = bytes([0,0,255, 0,255,0])
    cmd = [
        exe_path, "--use-pipe", "--verbose",
        "--input-width", str(input_w), "--input-height", str(input_h),
        "--input-channels", str(input_channels), "--pixel-format", "BGR",
        "-s", str(scale), "-m", models_dir_arg, "-n", REALESRGAN_MODEL_NAME_STEM_FOR_TEST
    ]
    process = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    _, stderr_data = process.communicate(input=raw_bgr_frame_incomplete, timeout=TIMEOUT_SECONDS)

    stderr_decoded = stderr_data.decode(errors='ignore')
    print(f"RealESRGAN Insufficient Stdin Test: CMD: {' '.join(cmd)}")
    print(f"RealESRGAN Insufficient Stdin Test: Stderr: {stderr_decoded}")
    assert process.returncode != 0, "RealESRGAN should have failed (insufficient stdin) but exited with 0."
    assert "could not read full frame from stdin" in stderr_decoded.lower()
