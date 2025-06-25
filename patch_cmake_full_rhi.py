import os
import re

def patch_cmake_for_full_rhi_support():
    """
    Patches Waifu2x-Extension-QT/CMakeLists.txt to add full RHI support.
    1. Adds 'RHI' to the find_package(Qt6 ...) call.
    2. Adds 'Qt6::RHI' to the target_link_libraries(...) call.
    """
    cmake_file_path = "Waifu2x-Extension-QT/CMakeLists.txt"

    if not os.path.exists(cmake_file_path):
        print(f"Error: Cannot find {cmake_file_path}")
        return

    print(f"Reading {cmake_file_path} to add full Qt6 RHI support...")

    with open(cmake_file_path, 'r') as f:
        content = f.read()

    original_content = content # Keep a copy for comparison

    # --- Step 1: Add RHI to find_package ---
    # Regex to find 'find_package(Qt6 ... COMPONENTS ...)' and capture up to just before the closing ')'
    # It handles cases where COMPONENTS might be followed by other keywords like REQUIRED or OPTIONAL
    find_package_search_pattern = r'(find_package\s*\(\s*Qt6.*?COMPONENTS\s*(?:[A-Za-z0-9_]+\s*)*?)(\s*REQUIRED|\s*OPTIONAL|\s*\))'

    def replace_find_package(match):
        components_part = match.group(1)
        ending_part = match.group(2)
        if "RHI" not in components_part:
            print("Patched find_package to include RHI component.")
            return components_part.rstrip() + " RHI" + ending_part
        print("Warning: find_package already contains RHI or pattern did not precisely match for adding RHI.")
        return match.group(0) # Return original match if RHI is there or other issue

    content, num_subs_find_package = re.subn(find_package_search_pattern, replace_find_package, content, 1, flags=re.IGNORECASE | re.DOTALL)
    if num_subs_find_package == 0 and "RHI" not in content: # A fallback check if the specific pattern above fails but RHI is missing
         alt_pattern = re.compile(r'(find_package\s*\(\s*Qt6.*?COMPONENTS[^\)]*)(\))', re.IGNORECASE | re.DOTALL)
         match_alt = alt_pattern.search(content)
         if match_alt and "RHI" not in match_alt.group(1):
             insertion_point = match_alt.start(2) # Insert before the final closing parenthesis
             content = content[:insertion_point].rstrip() + " RHI " + content[insertion_point:]
             print("Patched find_package (alternative) to include RHI component.")
         elif "RHI" not in content: # Crude check if still not there
             print("Warning: RHI component could not be added to find_package. Manual check needed if RHI errors persist.")


    # --- Step 2: Add Qt6::RHI to target_link_libraries ---
    # Regex to find 'target_link_libraries(Beya_Waifu ...)' and capture up to just before the closing ')'
    target_link_search_pattern = r'(target_link_libraries\s*\(\s*Beya_Waifu\b[^\)]*)(\))'

    def replace_target_link(match):
        libs_part = match.group(1)
        closing_paren = match.group(2)
        if "Qt6::RHI" not in libs_part:
            print("Patched target_link_libraries to include Qt6::RHI.")
            # Add on a new indented line for readability
            return libs_part.rstrip() + "\n    Qt6::RHI\n" + closing_paren
        print("Warning: target_link_libraries already contains Qt6::RHI or pattern did not precisely match for adding Qt6::RHI.")
        return match.group(0)

    content, num_subs_target_link = re.subn(target_link_search_pattern, replace_target_link, content, 1, flags=re.IGNORECASE | re.DOTALL)
    if num_subs_target_link == 0 and "Qt6::RHI" not in content: # A crude check if still not there
        print("Warning: Qt6::RHI could not be added to target_link_libraries. Manual check needed if RHI errors persist.")

    if content != original_content:
        with open(cmake_file_path, 'w') as f:
            f.write(content)
        print("CMake RHI patch script finished, changes were made.")
    else:
        print("CMake RHI patch script finished, no changes made (RHI might have been present or patterns didn't match).")


if __name__ == "__main__":
    patch_cmake_for_full_rhi_support()
