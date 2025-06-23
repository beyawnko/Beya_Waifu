import os

COPYRIGHT_NOTICE = """/*
    Copyright (C) 2025  beyawnko

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    My Github homepage: https://github.com/beyawnko
*/

"""

def add_copyright(file_path):
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()

        # Check if copyright notice already exists
        if "Copyright (C)" in content[:500] and "beyawnko" in content[:500]:
            print(f"Copyright notice already exists in {file_path}. Skipping.")
            return

        # Handle existing file comment like "// file: anime4kprocessor.h"
        if content.strip().startswith("// file:"):
            first_line_end = content.find('\\n')
            if first_line_end != -1:
                file_comment = content[:first_line_end+1]
                rest_of_content = content[first_line_end+1:]
                new_content = file_comment + "\\n" + COPYRIGHT_NOTICE + rest_of_content
            else: # file only has one line
                new_content = content + "\\n\\n" + COPYRIGHT_NOTICE
        else:
            new_content = COPYRIGHT_NOTICE + content

        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"Added copyright notice to {file_path}")

    except Exception as e:
        print(f"Error processing {file_path}: {e}")

if __name__ == "__main__":
    files_to_update = [
        "Waifu2x-Extension-QT/anime4kprocessor.h",
        "Waifu2x-Extension-QT/anime4kprocessor.cpp"
    ]
    for f_name in files_to_update:
        if os.path.exists(f_name):
            add_copyright(f_name)
        else:
            print(f"File not found: {f_name}")
