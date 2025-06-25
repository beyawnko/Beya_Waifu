import os

def patch_syntax_error_v2():
    """
    Patches realesrganprocessor.cpp to comment out an extraneous line.
    This version uses a substring search for robustness.
    """
    cpp_file_path = "Waifu2x-Extension-QT/realesrganprocessor.cpp"

    if not os.path.exists(cpp_file_path):
        print(f"Error: Cannot find {cpp_file_path}")
        return

    print(f"Reading {cpp_file_path} to fix syntax error (v2)...")

    with open(cpp_file_path, 'r') as f:
        lines = f.readlines()

    # The key part of the string causing the error
    offending_substring = "[end of Waifu2x-Extension-QT/realesrganprocessor.cpp]"
    found = False

    for i, line in enumerate(lines):
        # Use 'in' for a more robust substring search
        if offending_substring in line:
            lines[i] = "// " + line
            found = True
            print(f"Found and commented out line {i+1}: {line.strip()}")
            break

    if found:
        with open(cpp_file_path, 'w') as f:
            f.writelines(lines)
        print("Patch applied successfully.")
    else:
        print(f"Warning: Offending substring not found in {cpp_file_path}. The file might be clean.")

if __name__ == "__main__":
    patch_syntax_error_v2()
