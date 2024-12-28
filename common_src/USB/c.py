def move_fragment_right(input_file, output_file, shift_position):
    with open(input_file, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    with open(output_file, 'w', encoding='utf-8') as file:
        for line in lines:
            # Ищем начало фрагмента "/*"
            fragment_start = line.find("/*")
            
            if fragment_start != -1:
                # Разделяем строку на две части: до и начиная с фрагмента "/*"
                before_fragment = line[:fragment_start].rstrip()  # Все, что до "/*", убираем лишние пробелы справа
                fragment = line[fragment_start:].rstrip()  # Фрагмент начиная с "/*" до конца строки
                
                # Вычисляем количество пробелов, чтобы фрагмент оказался на нужной позиции
                space_count = shift_position - len(before_fragment)
                
                if space_count > 0:
                    # Добавляем нужное количество пробелов между частью до "/*" и самим фрагментом
                    new_line = before_fragment + ' ' * space_count + fragment
                else:
                    # Если фрагмент уже находится правее или слишком близко, не добавляем пробелы
                    new_line = before_fragment + ' ' + fragment
                
                # Записываем изменённую строку
                file.write(new_line + '\n')
            else:
                # Если в строке нет "/*", записываем её без изменений
                file.write(line)

# Пример использования
input_file = 'USB_descriptors.h'
output_file = '_USB_descriptors.h'
shift_position = 40  # Позиция для перемещения фрагмента
move_fragment_right(input_file, output_file, shift_position)
