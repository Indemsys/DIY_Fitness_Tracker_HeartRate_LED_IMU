import re
import xml.etree.ElementTree as ET

def extract_vectors(map_file_path):
    vectors = []
    vector_name = ""
    in_vector = False

    # Регулярное выражение для начала вектора
    vector_start_pattern = re.compile(r'\.vector\.(\S+)')
    # Регулярное выражение для строки, содержащей адрес и размер (то есть конец описания вектора)
    vector_end_pattern = re.compile(r'\s+const\s+0x[0-9a-fA-F\' ]+\s+0x[0-9a-fA-F]+')

    with open(map_file_path, 'r') as map_file:
        lines = map_file.readlines()

    for line in lines:
        if in_vector:
            match_end = vector_end_pattern.search(line)
            if match_end:
                vectors.append(vector_name.replace(".vector.", "").strip())  # Убираем префикс .vector.
                vector_name = ""
                in_vector = False
            else:
                vector_name += line.strip()
        else:
            if '.vector_info' not in line and not line.strip().startswith('"'):
                match_start = vector_start_pattern.search(line)
                if match_start:
                    vector_name = match_start.group(0)
                    match_end = vector_end_pattern.search(line)
                    if match_end:
                        vectors.append(vector_name.replace(".vector.", "").strip())
                        vector_name = ""
                    else:
                        in_vector = True
    return vectors

def update_svd_file(svd_file_path, vectors):
    # Чтение и разбор XML файла
    tree = ET.parse(svd_file_path)
    root = tree.getroot()

    # Поиск и обновление всех блоков <interrupt>
    for interrupt in root.iter('interrupt'):
        value_tag = interrupt.find('value')
        name_tag = interrupt.find('name')

        if value_tag is not None and name_tag is not None:
            try:
                # Номер прерывания
                interrupt_number = int(value_tag.text)
                # Проверяем, есть ли имя прерывания в списке, соответствующее номеру
                if 0 <= interrupt_number < len(vectors):
                    name_tag.text = vectors[interrupt_number]  # Замена имени на новое
            except ValueError:
                continue  # Если значение в <value> не число, пропускаем

    # Сохранение обновленного файла
    tree.write(svd_file_path, encoding='utf-8', xml_declaration=True)

# Пример использования
map_file_path = 'out/FTHRLI/List/S7V30_FTHRLI.map'  # Путь к .map файлу
svd_file_path = 'R7FS7G2x.svd'  # Путь к XML файлу
vectors = extract_vectors(map_file_path)  # Извлечение векторов из .map файла
update_svd_file(svd_file_path, vectors)  # Обновление XML файла

print(f"Имена прерываний успешно обновлены в файле {svd_file_path}")
