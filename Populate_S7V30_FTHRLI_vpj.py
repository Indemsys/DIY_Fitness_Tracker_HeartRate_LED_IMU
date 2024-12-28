import os
import xml.etree.ElementTree as ET


xml_file_path  = 'S7V30_FTHRLI.vpj'
root_directory = './'


# Список разрешенных расширений
ALLOWED_EXTENSIONS = ['.h', '.c', '.s', '.S', '.cpp', '.hpp' , '.a', 'icf']

# Список запрещенных директорий
BANNED_DIRECTORIES = ['out', 'ParametersGenerator', 'settings']

def delete_existing_structure(tree):
    root = tree.getroot()
    for files_element in root.findall('Files'):
        if files_element.get('AutoFolders') == 'DirectoryView':
            root.remove(files_element)
            break

def create_folder_structure(parent_element, current_dir, root_dir, allowed_extensions, banned_dirs):
    # Проверяем, не является ли текущая директория запрещенной
    if os.path.basename(current_dir) in banned_dirs:
        return

    # Пропускаем корневую директорию для первого вызова
    if current_dir != root_dir:
        folder_name = os.path.basename(current_dir)
        folder_element = ET.SubElement(parent_element, 'Folder', Name=folder_name)
    else:
        folder_element = parent_element

    for item in os.listdir(current_dir):
        item_path = os.path.join(current_dir, item)
        
        if os.path.isdir(item_path) or os.path.islink(item_path):
            # Рекурсивно создаем структуру для вложенных директорий, включая символические ссылки
            create_folder_structure(folder_element, item_path, root_dir, allowed_extensions, banned_dirs)
        elif os.path.isfile(item_path):
            # Проверяем расширение файла
            if any(item.endswith(ext) for ext in allowed_extensions):
                relative_path = os.path.relpath(item_path, root_dir)
                ET.SubElement(folder_element, 'F', N=relative_path)

def generate_new_structure(root_dir, allowed_extensions, banned_dirs):
    files_element = ET.Element('Files', AutoFolders="DirectoryView")
    create_folder_structure(files_element, root_dir, root_dir, allowed_extensions, banned_dirs)
    return files_element

def update_xml_file(xml_path, root_dir, allowed_extensions, banned_dirs):
    tree = ET.parse(xml_path)
    
    # Удаляем существующую структуру
    delete_existing_structure(tree)
    
    # Генерируем новую структуру
    new_files_element = generate_new_structure(root_dir, allowed_extensions, banned_dirs)
    
    # Добавляем новую структуру к корню документа
    tree.getroot().append(new_files_element)
    
    # Сохраняем обновленный XML
    tree.write(xml_path, encoding='utf-8', xml_declaration=True)

if __name__ == "__main__":

    
    update_xml_file(xml_file_path, root_directory, ALLOWED_EXTENSIONS, BANNED_DIRECTORIES)
