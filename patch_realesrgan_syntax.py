import os

def patch_syntax_error():
    """
    Patches realesrganprocessor.cpp to comment out an extraneous line
    that is causing a syntax error during compilation.
    """
    cpp_file_path = "Waifu2x-Extension-QT/realesrganprocessor.cpp"

    if not os.path.exists(cpp_file_path):
        print(f"Error: Cannot find {cpp_file_path}")
        return

    print(f"Reading {cpp_file_path} to fix syntax error...")

    with open(cpp_file_path, 'r') as f:
        lines = f.readlines()

    # The exact line causing the error
    offending_line_strip = "[end of Waifu2x-Extension-QT/realesrganprocessor.cpp]"
    found = False

    for i, line in enumerate(lines):
        if line.strip() == offending_line_strip: # Compare stripped versions
            lines[i] = "// " + line
            found = True
            print(f"Commenting out line {i+1}: {line.strip()}")
            break

    if found:
        with open(cpp_file_path, 'w') as f:
            f.writelines(lines)
        print("Patch applied successfully.")
    else:
        print(f"Warning: Offending line '{offending_line_strip}' not found in {cpp_file_path}. It may have been fixed already.")

if __name__ == "__main__":
    patch_syntax_error()
