import os

def patch_files():
    """
    Applies patches to mainwindow.h and mainwindow.cpp to fix compilation errors.
    """
    # Define file paths
    h_file_path = "Waifu2x-Extension-QT/mainwindow.h"
    cpp_file_path = "Waifu2x-Extension-QT/mainwindow.cpp"

    # --- Patch mainwindow.h ---
    try:
        with open(h_file_path, 'r') as f:
            h_content = f.read()

        # Define the incorrect and correct lines
        old_h_line = "  void onProcessingFinished(int rowNum, bool success);"
        # Ensure the new line maintains similar leading whitespace to the original context if possible
        # Based on the provided mainwindow.h, the original line had two spaces.
        new_h_line = "  void onProcessingFinished(int rowNum, bool success, ProcessJobType jobType);"

        if old_h_line in h_content:
            # Replace the old line with the new one
            h_content = h_content.replace(old_h_line, new_h_line)
            with open(h_file_path, 'w') as f:
                f.write(h_content)
            print(f"Successfully patched {h_file_path}")
        else:
            print(f"Warning: Line to patch not found in {h_file_path}. It might already be patched or the search string is incorrect (e.g. whitespace).")
            # Attempting a more flexible search for the .h file
            import re
            # Regex to find the line, allowing for slight variations in whitespace at the beginning
            # and ensuring it's the specific function declaration we're looking for.
            pattern = re.compile(r"^\s*void\s+onProcessingFinished\s*\(\s*int\s+rowNum\s*,\s*bool\s+success\s*\)\s*;\s*$", re.MULTILINE)
            if pattern.search(h_content):
                h_content = pattern.sub(new_h_line, h_content)
                with open(h_file_path, 'w') as f:
                    f.write(h_content)
                print(f"Successfully patched {h_file_path} using regex fallback.")
            else:
                print(f"Error: Line to patch still not found in {h_file_path} even with regex. Please check the file content and search string.")
                return # Stop if .h patch fails

    except FileNotFoundError:
        print(f"Error: {h_file_path} not found.")
        return

    # --- Patch mainwindow.cpp ---
    try:
        with open(cpp_file_path, 'r') as f:
            cpp_content = f.read()

        # Define the incorrect and correct parts for mainwindow.cpp
        # Original: if (ui->checkBox_BilateralFilterFaster_Post_Anime4k->isChecked()) preFilterParts << "bilateralFilterFaster";
        old_cpp_str = 'preFilterParts << "bilateralFilterFaster";'
        new_cpp_str = 'postFilterParts << "bilateralFilterFaster";'

        if old_cpp_str in cpp_content:
            # Replace the typo
            cpp_content = cpp_content.replace(old_cpp_str, new_cpp_str)
            with open(cpp_file_path, 'w') as f:
                f.write(cpp_content)
            print(f"Successfully patched {cpp_file_path}")
        else:
            print(f"Warning: String to patch not found in {cpp_file_path}. It might already be patched or the search string is incorrect.")
            # For the .cpp file, the exact string match is usually more reliable for such a specific typo.
            # If it's not found, manual inspection might be needed if the file content has diverged.
            # We won't add a regex fallback for this specific typo correction unless necessary.
            # If the script reports this warning, the user should be notified to check mainwindow.cpp around line 315.

    except FileNotFoundError:
        print(f"Error: {cpp_file_path} not found.")
        return

if __name__ == "__main__":
    patch_files()
