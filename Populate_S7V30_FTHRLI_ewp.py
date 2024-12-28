import os
import string
import xml.etree.ElementTree as ET

prog_file_name = "S7V30_FTHRLI.ewp"
proj_dir = os.getcwd() # Файл должен находится в корневой директории проекта


excluded_dirs = [".", 
                 ".hg",
                 ".settings", 
                 "settings", 
                 "Out", 
                 "debug", 
                 "release", 
                 ]

included_extensions = {'.c', '.h', '.s', '.S', '.a' ,'.cpp', '.hpp'}


#------------------------------------------------------------------------------------------------------------
# Функция рекурсивного прохода вглубь по директориям с корнем заданным в аргументе root_path и создание субблоков в дереве xml_tree 
#------------------------------------------------------------------------------------------------------------
def Create_IAR_groups_tree(current_path, xml_tree):
  n = 0

  # Создаем группу
  iar_group = ET.SubElement(xml_tree, 'group') # Создаем XML тэг group в дереве переданном в качестве аргумента xml_tree - это будет блок содержащий элементы каталога в дереве IAR
  name_tag = ET.SubElement(iar_group, 'name')  # В тэге group создаем тэг name - это будет именем группы в дереве каталогов IAR
  name_tag.text = os.path.basename(current_path)  # Получаем полный путь к текущей директории

  # Рекурсивно проходим по дереву директорий 
  dirs_iterator = os.scandir(current_path)
  for directory_object in dirs_iterator:
      deeper_path = os.path.join(current_path, directory_object.name)
        
      # Проверка на наличие директории в списке исключений
      if (not any(excl_dir == directory_object.name for excl_dir in excluded_dirs) 
          and not directory_object.name.startswith('.') 
          and directory_object.is_dir()):
            
          print('Group: ' + deeper_path)
          n = n + Create_IAR_groups_tree(deeper_path, iar_group)  # Рекурсивно уходим в глубь дерева директорий

  # Обрабатываем файлы в текущей директории
  dirs_iterator = os.scandir(current_path)
  for directory_object in dirs_iterator:
     if not directory_object.name.startswith('.') and directory_object.is_file():
        fname = os.path.join(current_path, directory_object.name)
            
          # Проверяем, заканчивается ли имя файла на разрешенное расширение
        if any(fname.endswith(ext) for ext in included_extensions):
           p = fname.replace(proj_dir, "$PROJ_DIR$")
           file_tag = ET.SubElement(iar_group, 'file')   # Вставляем тэг file в текущее дерево группы
           name_tag = ET.SubElement(file_tag, 'name')    # Вставляем в блок тэга file блок name с именем файла  
           name_tag.text = p
           n = n + 1  
           print('File:  ' + p)


  if n==0:
    xml_tree.remove(iar_group) # Если не было записано ни одного файла, то уже вставленый каталог удаляем  

  ET.indent(iar_group, space=" ", level=0)
  return n

#============================================================================================================
# Начало выполнения программы
#============================================================================================================

full_proj_name = proj_dir + "\\" + prog_file_name  # Получаем полный путь к файлу проекта IAR

# Открываем файл проекта

tree = ET.parse(full_proj_name) # Парсим XML файл проекта IAR

xml_root = tree.getroot() 
  
XMLblock_configuration = xml_root.findall('configuration') # Находим блок с конфигурацией 


#
#  Начинаем с того что убираем старые и устанавливаем новые пути к .h файлам 
#  
#  Находим и обрабатываем такие структуры
#  <settings>
#    <name>ICCARM</name> - Интересующая структура в кторой есть структура с путями к .h файлам 
#    <data>
#      <option>
#      ....
#      </option>
#      
#      <option>
#        <name>CCIncludePath2</name>  - Интересующая структура, кторую надо заменить 
#        <state>$PROJ_DIR$\src</state>
#        <state>....</state>
#        <state>....</state>  
#      ....
#      
#      </option>
#    </data>
#  </settings>

#............................................................................................................
# Итерация по всем инересующим блокам где могут находится обяъвления путей посика .h файлов 
for XMLblock_configuration_item  in XMLblock_configuration:

  for XMLblock_settings in XMLblock_configuration_item.iter('settings'):
    XMLblock_name = XMLblock_settings.find('name')

    # Находим блок содержащий все опции текущей конфигурации <configuration>...<settings>...<name>ICCARM</name>..<data> ....  </data>...</settings>... </configuration>

    if XMLblock_name.text == 'ICCARM':
      XMLblock_data = XMLblock_settings.find('data')
      break

  # Преобразуем список подключаемых путей так чтобы в него попали все поддиректории корня проекта


  # Ищем и удаляем блок CCIncludePath2 с перечислением подключаемых путей
  for XMLblock_option in XMLblock_data.iter('option'):
    XMLblock_option_name = XMLblock_option.find('name')
    if XMLblock_option_name.text == 'CCIncludePath2':
      XMLblock_data.remove(XMLblock_option)
      break


  # Создаем заново блок  <option> с перечислением путей 
  new_XMLblock_option = ET.SubElement(XMLblock_data, 'option')     # Создаем новый блок new_XMLblock_option в блоке XMLblock_data = ICCARM
  new_XMLblock_name = ET.SubElement(new_XMLblock_option, 'name')   # Создаем новый блок new_XMLblock_name в блоке new_XMLblock_option
  new_XMLblock_name.text = 'CCIncludePath2'                        # Блоку new_XMLblock_name даем имя CCIncludePath2 




  # Итерация по всем директориям в proj_dir
  for dirpath, dirs, files in os.walk(proj_dir, followlinks=True):
    # Проверка, не содержит ли путь любую из исключаемых директорий
    if not any(os.path.normpath(excl_dir) in os.path.normpath(dirpath).split(os.sep) for excl_dir in excluded_dirs) and dirpath != proj_dir:
        # Проверка, содержит ли текущая директория файлы с нужными расширениями
        if any(file.endswith(tuple(included_extensions)) for file in files):
            p = dirpath.replace(proj_dir, "$PROJ_DIR$")
            new_incl = ET.SubElement(new_XMLblock_option, 'state')
            new_incl.text = p
            print('Include: ' + p)


  ET.indent(new_XMLblock_option, space=" ", level=0)

  
# Конец итерации по конфигурациям for configuration_object  in xml_configuration_block:
#............................................................................................................
   
 
 
# Здесь убираем старый и записываем новый список .c .h .s .a файлов проекта  
#  
# Удаляем список груп и файлов в группах и формируем новый с распределением по группам аналогичным распределению по директориям
# Список файлов и груп в проект один для всех конфигураций

block_group = xml_root.findall('group')
for block_group_item  in block_group:
  xml_root.remove(block_group_item)
  
  
Create_IAR_groups_tree(proj_dir , xml_root)  
  
    

print ("END!")
tree.write(full_proj_name, method="xml", xml_declaration=True, short_empty_elements=False, encoding="UTF-8")


