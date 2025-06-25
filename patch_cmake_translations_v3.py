import os
import re

def patch_cmake_for_english_only_v3():
    cmake_file_path = "Waifu2x-Extension-QT/CMakeLists.txt"

    if not os.path.exists(cmake_file_path):
        print(f"Error: Cannot find {cmake_file_path}")
        return

    print(f"Reading {cmake_file_path} to modify for English-only translations (v3)...")

    with open(cmake_file_path, 'r') as f:
        lines = f.readlines()

    new_lines = []

    in_project_translations_ts_block = False
    in_qrc_content_static_block = False

    project_translations_ts_start_pattern = re.compile(r'set\s*\(\s*PROJECT_TRANSLATIONS_TS')
    qrc_content_static_start_pattern = re.compile(r'set\s*\(\s*TRANSLATIONS_QRC_FILE_CONTENT_STATIC')
    # A more robust way to find the end of a multi-line set() command is to count parentheses
    # but for this specific structure, checking for a line that primarily contains ')' should work.
    # This assumes no other ')' appear at the start of lines within the set block.
    set_block_end_pattern = re.compile(r'^\s*\)\s*$')


    ts_file_to_comment_pattern = re.compile(r'^\s*language_(?!English).*\.ts\s*$', re.IGNORECASE)
    # Regex to find .qm <file> lines to comment out (non-English)
    # It captures the whole line to wrap it in an XML comment
    qm_file_line_to_comment_pattern = re.compile(r'^(\s*<file>language_(?!English).*\.qm</file>\s*)$', re.IGNORECASE)

    for line in lines:
        # Handling PROJECT_TRANSLATIONS_TS block
        if project_translations_ts_start_pattern.search(line):
            in_project_translations_ts_block = True
            new_lines.append(line)
            continue

        if in_project_translations_ts_block:
            if ts_file_to_comment_pattern.search(line):
                new_lines.append("#" + line) # CMake comment for .ts list
                print(f"Commenting out .ts line: {line.strip()}")
            else:
                new_lines.append(line)
            if set_block_end_pattern.search(line.strip()): # Check if this line is just ')'
                in_project_translations_ts_block = False
            continue

        # Handling TRANSLATIONS_QRC_FILE_CONTENT_STATIC block
        if qrc_content_static_start_pattern.search(line):
            in_qrc_content_static_block = True
            new_lines.append(line)
            continue

        if in_qrc_content_static_block:
            match = qm_file_line_to_comment_pattern.search(line)
            if match:
                # original_line = match.group(1) # The whole matched line
                # Using XML comment markers. Need to be careful if the string is inside CMake quotes.
                # The string itself is quoted in CMake, so # is fine for CMake, but not for XML.
                # The script should modify the *content* of the CMake string.
                # This means the line being added to new_lines should be `<!--` + line + `-->`
                # but this must be done carefully considering CMake's own string quoting.
                # A simpler approach for the script: comment the line in CMake itself if it's simple.
                # However, the content is a multi-line string.
                # The script should ideally parse the string content, modify it, then reconstruct the CMake set() command.
                # For now, let's try to make the Python script generate valid XML comment *within the string data*.

                # The line is like: "    <file>language_Chinese.qm</file>\n"
                # We want it to become: "    <!-- <file>language_Chinese.qm</file> -->\n"
                # The # character was causing issues for RCC.
                # The line is part of a CMake string literal.
                # So, the output line in the CMakeLists.txt should be:
                # "    <!-- <file>language_Chinese.qm</file> -->\n"
                # This is fine. The previous script just prepended # to this line.

                # Correct approach: The Python script should change the *content* of the string
                # that CMake writes to the .qrc file.
                # So, if a line in lines[] is part of that string, modify it.
                new_lines.append(f'{line.rstrip()[:line.find("<file>")]}<!-- {line.strip()} -->\n')
                print(f"XML-Commenting out .qm line in QRC content: {line.strip()}")
            else:
                new_lines.append(line)

            if '</RCC>"' in line: # End of the QRC string content
                # Check if the next line is the closing parenthesis for the set command
                pass # Stay in block, the ')' will terminate it
            if line.strip() == ')' and not qrc_content_static_start_pattern.search(line) : # End of the set command
                in_qrc_content_static_block = False
            continue

        new_lines.append(line)

    with open(cmake_file_path, 'w') as f:
        f.writelines(new_lines)

    print(f"V3 Patch applied successfully to {cmake_file_path}.")

if __name__ == "__main__":
    patch_cmake_for_english_only_v3()
