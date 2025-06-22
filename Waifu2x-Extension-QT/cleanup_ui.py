# file: Waifu2x-Extension-QT/cleanup_ui.py
import xml.etree.ElementTree as ET

# This script is designed to be run from the 'Waifu2x-Extension-QT' directory.
UI_FILE_PATH = 'mainwindow.ui'

def remove_css_class_properties(file_path):
    """
    Parses a .ui file, removes all <dynamicProperty name="cssClass"> tags,
    and overwrites the original file with the cleaned-up version.
    """
    try:
        # Registering the namespace can help with some tools, but for this simple structure, it's often not needed.
        # We will parse without it and handle namespaces implicitly if they cause issues.
        tree = ET.parse(file_path)
        root = tree.getroot()

        # Find all <widget> elements anywhere in the tree
        widgets = root.findall('.//widget')

        nodes_removed_count = 0

        # Iterate over each widget to find the target dynamicProperty
        for widget in widgets:
            properties_to_remove = []
            for child in widget:
                # Check if the child is a dynamicProperty and has the name 'cssClass'
                if child.tag == 'dynamicProperty' and child.attrib.get('name') == 'cssClass':
                    properties_to_remove.append(child)

            # Remove the collected nodes from the widget
            for prop in properties_to_remove:
                widget.remove(prop)
                nodes_removed_count += 1

        # Write the modified tree back to the file, preserving the XML declaration
        tree.write(file_path, encoding='UTF-8', xml_declaration=True)

        print(f"Successfully processed '{file_path}'.")
        print(f"Removed {nodes_removed_count} instances of <dynamicProperty name=\"cssClass\">.")

    except ET.ParseError as e:
        print(f"Error parsing XML file: {e}")
    except FileNotFoundError:
        print(f"Error: The file '{file_path}' was not found.")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")

if __name__ == "__main__":
    remove_css_class_properties(UI_FILE_PATH)
