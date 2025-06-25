import os
import re

def patch_cmake_for_english_only():
    """
    Patches Waifu2x-Extension-QT/CMakeLists.txt to effectively process only English translations
    by commenting out non-English .ts files from the PROJECT_TRANSLATIONS_TS set command.
    """
    cmake_file_path = "Waifu2x-Extension-QT/CMakeLists.txt"

    if not os.path.exists(cmake_file_path):
        print(f"Error: Cannot find {cmake_file_path}")
        return

    print(f"Reading {cmake_file_path} to modify for English-only translations...")

    with open(cmake_file_path, 'r') as f:
        lines = f.readlines()

    new_lines = []
    in_translation_set_block = False # Flag to know if we are inside set(PROJECT_TRANSLATIONS_TS ...)

    # Regex to find .ts files, excluding the English one
    # It looks for lines that start with optional whitespace, then "language_",
    # then anything NOT "English", then any characters, then ".ts", then optional whitespace and newline.
    ts_file_pattern = re.compile(r'^\s*language_(?!English).*\.ts\s*$', re.IGNORECASE)

    # Pattern to identify the start of the specific set command
    set_block_start_pattern = re.compile(r'set\s*\(\s*PROJECT_TRANSLATIONS_TS')
    # Pattern to identify the end of a set command block (a line ending with ')')
    set_block_end_pattern = re.compile(r'\)\s*$')


    for line_number, line in enumerate(lines):
        if set_block_start_pattern.search(line):
            in_translation_set_block = True
            new_lines.append(line)
            continue

        if in_translation_set_block:
            if ts_file_pattern.search(line):
                new_lines.append("#" + line) # Comment out the non-English .ts file
                print(f"Commenting out line: {line.strip()}")
            else:
                new_lines.append(line)

            # Check if this line ends the set(PROJECT_TRANSLATIONS_TS ...) block
            # A simple check for a line that contains ')' and is not the start line itself
            # More robust parsing might be needed for very complex CMake files, but this should work here.
            if ')' in line and not set_block_start_pattern.search(line):
                 # Check if the ')' is likely the closing parenthesis of the set command
                 # This heuristic assumes the set command for translations isn't extremely complex
                 # with many nested parentheses on separate lines.
                 # A more precise way would be to count open/close parentheses from the start of the block.
                 # For this specific file structure, this simple check should be okay.
                 if line.strip().endswith(')'):
                    in_translation_set_block = False
        else:
            new_lines.append(line)

    with open(cmake_file_path, 'w') as f:
        f.writelines(new_lines)

    print("Patch applied successfully to Waifu2x-Extension-QT/CMakeLists.txt for English-only translations.")

if __name__ == "__main__":
    patch_cmake_for_english_only()
