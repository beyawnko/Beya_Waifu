import re
import sys

def read_file_content(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        return f.read()

def write_file_content(file_path, content):
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)

def replace_in_file_regex(file_path, pattern, replacement):
    content = read_file_content(file_path)
    new_content, num_replacements = re.subn(pattern, replacement, content, flags=re.DOTALL)
    if num_replacements > 0:
        write_file_content(file_path, new_content)
        print(f"Successfully replaced {num_replacements} occurrences in {file_path}")
        return True
    print(f"No replacements made in {file_path}")
    return False

if __name__ == "__main__":
    action = sys.argv[1]
    file_path = sys.argv[2]

    if action == "replace_regex":
        pattern = sys.argv[3]
        replacement = sys.argv[4]
        replace_in_file_regex(file_path, pattern, replacement)
    else:
        print(f"Unknown action: {action}")
        sys.exit(1)
