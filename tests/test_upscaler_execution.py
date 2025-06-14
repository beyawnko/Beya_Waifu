import subprocess
import os
import sys # For platform check

# Define the expected directory where the main application and upscaler binaries are located.
# This should be relative to the repository root where pytest is typically run.
APP_DIR = "Waifu2x-Extension-QT"

# Define the names of the upscaler executables.
# Add .exe for Windows, no extension for Linux/macOS.
# The build_projects.sh script produces .exe files for Windows (MSYS/CYGWIN).
# On Linux, the build would produce executables without .exe.
# For this test, we'll assume we're testing the Windows build output as per the context.
# If testing on Linux, these would need to be adjusted, or the test made platform-aware.

# For now, let's assume the test runs in an environment where .exe is expected,
# as the problem description mentions .exe files specifically.
# If this test were to run on Linux AFTER a Linux build, the names would be different.
RCUGAN_EXE_NAME = "realcugan-ncnn-vulkan.exe"
RESRGAN_EXE_NAME = "realesrgan-ncnn-vulkan.exe"

# Timeout for subprocess execution
TIMEOUT_SECONDS = 20

def test_realcugan_executable_existence_and_run():
    """
    Tests if the RealCUGAN executable exists and runs successfully, printing device info.
    """
    exe_path = os.path.join(APP_DIR, RCUGAN_EXE_NAME)

    print(f"Checking for RealCUGAN at: {os.path.abspath(exe_path)}")
    assert os.path.isfile(exe_path), f"{RCUGAN_EXE_NAME} not found at {exe_path}"

    try:
        # ncnn executables usually list devices and exit 0 when run with no arguments.
        # On Windows, they might open a console window briefly.
        # The `creationflags` with `subprocess.CREATE_NO_WINDOW` can hide the window on Windows
        # but it's not strictly necessary for the test to pass if the window briefly appears.
        # For cross-platform, just running it is fine.
        process_kwargs = {
            "capture_output": True,
            "text": True,
            "timeout": TIMEOUT_SECONDS,
            "check": False  # We check returncode manually
        }
        if sys.platform == "win32":
             # Optional: Hide console window on Windows. Requires Python 3.7+ for CREATE_NO_WINDOW
             # process_kwargs["creationflags"] = subprocess.CREATE_NO_WINDOW
             pass


        result = subprocess.run([exe_path], **process_kwargs)

        assert result.returncode == 0, \
            f"{RCUGAN_EXE_NAME} exited with {result.returncode}. Stderr: '{result.stderr}'. Stdout: '{result.stdout}'"

        # Check if stdout or stderr contains "device" (case-insensitive)
        output_combined = result.stdout.lower() + result.stderr.lower()
        assert "device" in output_combined, \
            f"{RCUGAN_EXE_NAME} did not print device info. Combined Output: '{output_combined}'"

    except FileNotFoundError:
        # This case should ideally be caught by os.path.isfile, but good for robustness.
        assert False, f"{RCUGAN_EXE_NAME} could not be found to run. Path: {exe_path}"
    except subprocess.TimeoutExpired:
        assert False, f"{RCUGAN_EXE_NAME} timed out after {TIMEOUT_SECONDS} seconds."

def test_realesrgan_executable_existence_and_run():
    """
    Tests if the RealESRGAN executable exists and runs successfully, printing device info.
    """
    exe_path = os.path.join(APP_DIR, RESRGAN_EXE_NAME)

    print(f"Checking for RealESRGAN at: {os.path.abspath(exe_path)}")
    assert os.path.isfile(exe_path), f"{RESRGAN_EXE_NAME} not found at {exe_path}"

    try:
        process_kwargs = {
            "capture_output": True,
            "text": True,
            "timeout": TIMEOUT_SECONDS,
            "check": False
        }
        if sys.platform == "win32":
            # process_kwargs["creationflags"] = subprocess.CREATE_NO_WINDOW
            pass

        result = subprocess.run([exe_path], **process_kwargs)

        assert result.returncode == 0, \
            f"{RESRGAN_EXE_NAME} exited with {result.returncode}. Stderr: '{result.stderr}'. Stdout: '{result.stdout}'"

        output_combined = result.stdout.lower() + result.stderr.lower()
        assert "device" in output_combined, \
            f"{RESRGAN_EXE_NAME} did not print device info. Combined Output: '{output_combined}'"

    except FileNotFoundError:
        assert False, f"{RESRGAN_EXE_NAME} could not be found to run. Path: {exe_path}"
    except subprocess.TimeoutExpired:
        assert False, f"{RESRGAN_EXE_NAME} timed out after {TIMEOUT_SECONDS} seconds."

if __name__ == "__main__":
    # This allows running the test directly using "python tests/test_upscaler_execution.py"
    # For pytest, it will discover the test_ functions automatically.

    print(f"Upscaler tests running. Expecting executables in: {os.path.abspath(APP_DIR)}")
    print(f"Platform: {sys.platform}")

    # Check if APP_DIR exists first, to give a clearer error if it doesn't.
    if not os.path.isdir(APP_DIR):
        print(f"ERROR: Application directory '{APP_DIR}' not found relative to current path '{os.getcwd()}'.")
        print("Please ensure the tests are run from the repository root, and the application has been built.")
        sys.exit(1)

    print("\nRunning test_realcugan_executable_existence_and_run...")
    try:
        test_realcugan_executable_existence_and_run()
        print("test_realcugan_executable_existence_and_run PASSED")
    except AssertionError as e:
        print(f"test_realcugan_executable_existence_and_run FAILED: {e}")
    except Exception as e:
        print(f"test_realcugan_executable_existence_and_run ERRORED: {e}")

    print("\nRunning test_realesrgan_executable_existence_and_run...")
    try:
        test_realesrgan_executable_existence_and_run()
        print("test_realesrgan_executable_existence_and_run PASSED")
    except AssertionError as e:
        print(f"test_realesrgan_executable_existence_and_run FAILED: {e}")
    except Exception as e:
        print(f"test_realesrgan_executable_existence_and_run ERRORED: {e}")

    print("\nAll upscaler execution tests completed (check output for PASS/FAIL/ERROR).")
