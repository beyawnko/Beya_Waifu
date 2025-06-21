import os
import re

# --- File Paths ---
project_dir = 'Waifu2x-Extension-QT'
ui_file = os.path.join(project_dir, 'mainwindow.ui')
fi_cpp = os.path.join(project_dir, 'Frame_Interpolation.cpp')
mw_cpp = os.path.join(project_dir, 'mainwindow.cpp')
style_qrc = os.path.join(project_dir, 'style.qrc') # Assume the stylesheet is findable via the .qrc

# --- Part 1: Fix the UI and Stylesheet definitively ---
print(f"Patching {ui_file} to use a safe property name...")
if os.path.exists(ui_file):
    with open(ui_file, 'r', encoding='utf-8') as f:
        content = f.read()
    # Replace all instances of styleClass with cssClass. This forces uic to use setProperty().
    # Also, ensure any original 'class' properties are also caught and changed to 'cssClass'.
    content = re.sub(r'name="(styleClass|class)"', r'name="cssClass"', content)

    # Remove tr="true" from any string within a cssClass property, just in case.
    # This is a bit more complex with regex if notr="true" is also a possibility.
    # A simpler approach for this specific problem is to ensure the problematic uic behavior isn't triggered.
    # The key is that setProperty("cssClass", "blue") will not involve QCoreApplication::translate.
    # So, ensuring the property name is not "class" and not one that uic might specially handle (like styleClass did)
    # is the primary fix. The python script already changes to cssClass.
    # For robustness, let's remove tr="true" if it's directly in a string tag of a cssClass property.
    content = re.sub(r'(<property name="cssClass">\s*<string)([^>]*) tr="true"([^>]*)>', r'\1\2\3>', content, flags=re.DOTALL)
    # And ensure notr="true" is added if no translation-related attribute is present.
    # This regex looks for <string> tags directly under cssClass that don't have notr or tr.
    # content = re.sub(r'(<property name="cssClass">\s*<string)(?![^>]*notr="true")(?![^>]*tr="true")([^>]*)>', r'\1 notr="true"\2>', content, flags=re.DOTALL)
    # Simpler: The rename to cssClass should be sufficient to avoid uic's special handling that involves QCoreApplication::translate.
    # The original script's approach of renaming to cssClass is the main fix for the .ui file.

    with open(ui_file, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"{ui_file} patched successfully.")
else:
    print(f"ERROR: Could not find {ui_file} to patch.")

# Proactively find and patch the QSS file. It's likely listed in style.qrc.
qss_file_path = None
if os.path.exists(style_qrc):
    with open(style_qrc, 'r', encoding='utf-8') as f:
        qrc_content = f.read()
    match = re.search(r'<file>([^<]+\.qss)</file>', qrc_content)
    if match:
        qss_file_path = os.path.join(project_dir, match.group(1))

if qss_file_path and os.path.exists(qss_file_path):
    print(f"Patching stylesheet {qss_file_path} to match UI property change...")
    with open(qss_file_path, 'r', encoding='utf-8') as f:
        qss_content = f.read()
    # Replace styleClass and class selectors with cssClass
    qss_content = re.sub(r'\[(styleClass|class)=([^\]]+)\]', r'[cssClass=\2]', qss_content)
    with open(qss_file_path, 'w', encoding='utf-8') as f:
        f.write(qss_content)
    print(f"{qss_file_path} patched successfully.")
else:
    print("Warning: Stylesheet (.qss) not found or specified in style.qrc. Manual review may be needed if styling breaks.")

# --- Part 2: Apply the correct C++ fixes ---
print(f"Patching {fi_cpp} for removed 'spinBox_retry'...")
if os.path.exists(fi_cpp):
    with open(fi_cpp, 'r', encoding='utf-8') as f:
        content = f.read()
    # Replace the call to the missing widget with a sensible default value of 1.
    content = re.sub(r'ui->spinBox_retry->value\(\)', '1', content)
    with open(fi_cpp, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"{fi_cpp} patched successfully.")

print(f"Patching {mw_cpp} to comment out obsolete language/update functions...")
if os.path.exists(mw_cpp):
     with open(mw_cpp, 'r', encoding='utf-8') as f:
        content = f.read()
     # Comment out references to the deleted widgets.
     content = re.sub(r'^(.*ui->comboBox_language.*)$', r'// \1', content, flags=re.MULTILINE)
     content = re.sub(r'^(.*ui->comboBox_UpdateChannel.*)$', r'// \1', content, flags=re.MULTILINE)
     with open(mw_cpp, 'w', encoding='utf-8') as f:
        f.write(content)
     print(f"{mw_cpp} patched successfully.")

# --- Part 3: Update Copyright Notices ---
print("Applying copyright and style guide updates...")
copyright_notice = """/*
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
*/"""

files_to_update = [fi_cpp, mw_cpp] # Add other cpp files if needed
# List all cpp files in Waifu2x-Extension-QT, excluding specific ones if necessary
all_cpp_files_in_project = []
for root, dirs, files in os.walk(project_dir):
    for file in files:
        if file.endswith(".cpp"):
            all_cpp_files_in_project.append(os.path.join(root, file))
# Add specific files from other directories if they exist and need update
# Example: launcher_main_cpp = os.path.join('Waifu2x-Extension-QT-Launcher', 'main.cpp')
# if os.path.exists(launcher_main_cpp):
#    all_cpp_files_in_project.append(launcher_main_cpp)

# Filter out files that might not need this generic update or are problematic
# For now, let's assume all .cpp files in Waifu2x-Extension-QT should be updated
# files_to_update = all_cpp_files_in_project

# The user script specified only fi_cpp and mw_cpp for copyright update. Let's stick to that for now.
# If a broader update is needed, files_to_update should be populated differently.

for file_path in files_to_update: # Using the original files_to_update list from user script
    if os.path.exists(file_path):
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()

        # Remove old copyright notice (make it more general)
        # This regex tries to find typical C-style block comments at the start of the file
        # that look like copyright notices.
        content = re.sub(r'^\s*/\*.*Copyright.*?My Github homepage:.*?\*/\s*', '', content, flags=re.DOTALL | re.MULTILINE)
        content = content.strip() # Remove leading/trailing whitespace after stripping old copyright

        # Add new copyright notice
        new_content = f"{copyright_notice}\n\n{content}"

        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"Updated copyright for {file_path}")
    else:
        print(f"Warning: Copyright update skipped, {file_path} not found.")

print("\n--- Definitive fixes applied. The build should now succeed. ---")
