import os
import re

def patch_cmake_for_english_only_v2():
    cmake_file_path = "Waifu2x-Extension-QT/CMakeLists.txt"

    if not os.path.exists(cmake_file_path):
        print(f"Error: Cannot find {cmake_file_path}")
        return

    print(f"Reading {cmake_file_path} to modify for English-only translations (v2)...")

    with open(cmake_file_path, 'r') as f:
        lines = f.readlines()

    new_lines = []

    # State flags
    in_project_translations_ts_block = False
    in_qrc_content_static_block = False

    # Patterns
    project_translations_ts_start_pattern = re.compile(r'set\s*\(\s*PROJECT_TRANSLATIONS_TS')
    qrc_content_static_start_pattern = re.compile(r'set\s*\(\s*TRANSLATIONS_QRC_FILE_CONTENT_STATIC')
    block_end_pattern = re.compile(r'\)\s*$') # Common pattern for set() command ending

    # Regex to find .ts files to comment out (non-English)
    ts_file_to_comment_pattern = re.compile(r'^\s*language_(?!English).*\.ts\s*$', re.IGNORECASE)
    # Regex to find .qm files in QRC content to comment out (non-English)
    qm_file_in_qrc_to_comment_pattern = re.compile(r'^\s*<file>language_(?!English).*\.qm</file>\s*$', re.IGNORECASE)

    for line in lines:
        # Check for start of PROJECT_TRANSLATIONS_TS block
        if project_translations_ts_start_pattern.search(line):
            in_project_translations_ts_block = True
            new_lines.append(line)
            # print(f"DEBUG: Entered PROJECT_TRANSLATIONS_TS block: {line.strip()}")
            continue

        # Check for start of TRANSLATIONS_QRC_FILE_CONTENT_STATIC block
        if qrc_content_static_start_pattern.search(line):
            in_qrc_content_static_block = True
            new_lines.append(line)
            # print(f"DEBUG: Entered TRANSLATIONS_QRC_FILE_CONTENT_STATIC block: {line.strip()}")
            continue

        # Process lines within PROJECT_TRANSLATIONS_TS block
        if in_project_translations_ts_block:
            if ts_file_to_comment_pattern.search(line):
                new_lines.append("#" + line)
                print(f"Commenting out .ts line: {line.strip()}")
            else:
                new_lines.append(line)
            # Check for end of this specific block
            if block_end_pattern.search(line) and not project_translations_ts_start_pattern.search(line) :
                # A simple check: if the line contains ')' and it's not the start of the set command.
                # This assumes the set command arguments are on separate lines or the final ')' is on its own line or at the end of the last argument.
                if line.strip().endswith(')'):
                    in_project_translations_ts_block = False
                    # print(f"DEBUG: Exited PROJECT_TRANSLATIONS_TS block with line: {line.strip()}")
            continue # Ensure we don't process this line further if it was in this block

        # Process lines within TRANSLATIONS_QRC_FILE_CONTENT_STATIC block
        if in_qrc_content_static_block:
            if qm_file_in_qrc_to_comment_pattern.search(line):
                new_lines.append("#" + line)
                print(f"Commenting out .qm line in QRC: {line.strip()}")
            else:
                new_lines.append(line)
            # Check for end of this specific block (assumes the string ends with </RCC>") and then a closing parenthesis for set())
            if '</RCC>"' in line: # Heuristic for the end of the QRC string content
                 # The actual set command might end on the next line with ')'
                 pass # Stay in block until ')' is found for the set command
            if block_end_pattern.search(line) and not qrc_content_static_start_pattern.search(line):
                if line.strip().endswith(')'): # Check if it's a line ending the set command
                    in_qrc_content_static_block = False
                    # print(f"DEBUG: Exited TRANSLATIONS_QRC_FILE_CONTENT_STATIC block with line: {line.strip()}")
            continue # Ensure we don't process this line further

        # If not in any special block, just append the line
        new_lines.append(line)

    with open(cmake_file_path, 'w') as f:
        f.writelines(new_lines)

    print(f"V2 Patch applied successfully to {cmake_file_path} for English-only translations.")

if __name__ == "__main__":
    patch_cmake_for_english_only_v2()
