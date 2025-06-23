import re
import sys

def patch_ui_file(input_file, output_file):
    """
    Patches the mainwindow.ui file to replace the QSplitter for file lists
    with a more user-friendly QTabWidget.
    """
    try:
        with open(input_file, 'r', encoding='utf-8') as f:
            content = f.read()
    except FileNotFoundError:
        print(f"Error: Input file '{input_file}' not found.", file=sys.stderr)
        sys.exit(1)

    # Find the QSplitter widget block
    # Use a greedy match for the content of splitter_FilesList
    splitter_regex = re.compile(r'<widget class="QSplitter" name="splitter_FilesList">.*</widget>', re.DOTALL)
    splitter_match = splitter_regex.search(content)

    if not splitter_match:
        print("Error: Could not find the QSplitter with name 'splitter_FilesList' in the UI file.", file=sys.stderr)
        return

    splitter_xml = splitter_match.group(0)

    # Extract the table views from within the splitter
    image_view_regex = re.compile(r'<widget class="QTableView" name="tableView_image">[\s\S]*?</widget>', re.DOTALL)
    gif_view_regex = re.compile(r'<widget class="QTableView" name="tableView_gif">[\s\S]*?</widget>', re.DOTALL)
    video_view_regex = re.compile(r'<widget class="QTableView" name="tableView_video">[\s\S]*?</widget>', re.DOTALL)

    image_view_match = image_view_regex.search(splitter_xml)
    gif_view_match = gif_view_regex.search(splitter_xml)
    video_view_match = video_view_regex.search(splitter_xml)

    if not all([image_view_match, gif_view_match, video_view_match]):
        print("Error: Could not find one or more required QTableView widgets inside the splitter.", file=sys.stderr)
        return

    image_view_xml = image_view_match.group(0)
    gif_view_xml = gif_view_match.group(0)
    video_view_xml = video_view_match.group(0)

    # Indent the extracted table views to match the new structure
    image_view_xml = "\n".join([" " * 21 + line for line in image_view_xml.splitlines()])
    gif_view_xml = "\n".join([" " * 21 + line for line in gif_view_xml.splitlines()])
    video_view_xml = "\n".join([" " * 21 + line for line in video_view_xml.splitlines()])

    # Create the new QTabWidget XML that will replace the splitter
    new_tab_widget_xml = f"""<widget class="QTabWidget" name="fileListTabWidget">
                  <property name="currentIndex">
                   <number>0</number>
                  </property>
                  <widget class="QWidget" name="tab_Images">
                   <attribute name="title">
                    <string>Images</string>
                   </attribute>
                   <layout class="QVBoxLayout" name="verticalLayout_ImagesTab">
                    <property name="leftMargin"><number>0</number></property>
                    <property name="topMargin"><number>0</number></property>
                    <property name="rightMargin"><number>0</number></property>
                    <property name="bottomMargin"><number>0</number></property>
                    <item>
                     {image_view_xml}
                    </item>
                   </layout>
                  </widget>
                  <widget class="QWidget" name="tab_Gifs">
                   <attribute name="title">
                    <string>GIF/APNG</string>
                   </attribute>
                   <layout class="QVBoxLayout" name="verticalLayout_GifsTab">
                    <property name="leftMargin"><number>0</number></property>
                    <property name="topMargin"><number>0</number></property>
                    <property name="rightMargin"><number>0</number></property>
                    <property name="bottomMargin"><number>0</number></property>
                    <item>
                     {gif_view_xml}
                    </item>
                   </layout>
                  </widget>
                  <widget class="QWidget" name="tab_Videos">
                   <attribute name="title">
                    <string>Videos</string>
                   </attribute>
                   <layout class="QVBoxLayout" name="verticalLayout_VideosTab">
                    <property name="leftMargin"><number>0</number></property>
                    <property name="topMargin"><number>0</number></property>
                    <property name="rightMargin"><number>0</number></property>
                    <property name="bottomMargin"><number>0</number></property>
                    <item>
                     {video_view_xml}
                    </item>
                   </layout>
                  </widget>
                 </widget>"""

    # Indent the replacement block to match the surrounding XML
    indentation = " " * 18
    new_tab_widget_xml = "\n".join([indentation + line for line in new_tab_widget_xml.splitlines()])

    # Replace the old splitter with the new tab widget
    new_content = content.replace(splitter_xml, new_tab_widget_xml)

    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(new_content)

    print(f"Successfully patched UI file: '{output_file}'")

if __name__ == '__main__':
    patch_ui_file('Waifu2x-Extension-QT/mainwindow.ui', 'Waifu2x-Extension-QT/mainwindow.ui')
