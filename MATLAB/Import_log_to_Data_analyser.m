% MATLAB скрипт для чтения двоичных данных и построения графиков акселерометра и гироскопа
% с игнорированием первой секунды данных и отображением RMS для каждого сигнала

% Частота дискретизации
fs = 104; % Гц

% Запрос файла у пользователя
[filename, pathname] = uigetfile('*.bin', 'Выберите двоичный файл данных');
if isequal(filename, 0)
    errordlg('Файл не выбран. Скрипт завершен.', 'Ошибка');
    return;
end
filepath = fullfile(pathname, filename);

% Открытие двоичного файла для чтения
fid = fopen(filepath, 'rb');
if fid == -1
    errordlg('Невозможно открыть указанный файл.', 'Ошибка');
    return;
end

% Чтение данных как int16
data = fread(fid, 'int16');

% Закрытие файла
fclose(fid);

% Проверка, что длина данных кратна 6
if mod(length(data), 6) ~= 0
    errordlg('Размер данных не кратен 6. Проверьте корректность файла данных.', 'Ошибка');
    return;
end

% Вычисление количества записей
num_records = length(data) / 6;

% Преобразование данных в матрицу N x 6
data = reshape(data, 6, num_records)';
% Столбцы соответствуют: [gyro_x, gyro_y, gyro_z, acc_x, acc_y, acc_z]

% Извлечение данных гироскопа и акселерометра
gyro_data = data(:, 1:3);
acc_data = data(:, 4:6);

% Создание временного вектора с учетом частоты дискретизации
t = (0:num_records-1)' / fs; % Время в секундах

% Игнорирование первой секунды данных
ignore_samples = fs; % Количество записей, соответствующее первой секунде
if num_records <= ignore_samples
    errordlg('Длина записи меньше или равна одной секунде. Недостаточно данных для обработки.', 'Ошибка');
    return;
end
gyro_data = gyro_data(ignore_samples+1:end, :);
acc_data = acc_data(ignore_samples+1:end, :);
t = t(ignore_samples+1:end);

% Расчет среднего значения для каждого сигнала
mean_acc = mean(acc_data);
mean_gyro = mean(gyro_data);

% Вычитание среднего значения из каждого сигнала
acc_data_centered = acc_data - mean_acc;
gyro_data_centered = gyro_data - mean_gyro;

% Расчет RMS для каждого сигнала относительно среднего уровня
rms_acc = rms(acc_data_centered);
rms_gyro = rms(gyro_data_centered);

% Создание фигуры на весь экран с панелью инструментов
figure('units','normalized','outerposition',[0 0 1 1], 'Toolbar', 'figure');

% Получение дескриптора панели инструментов
htoolbar = findall(gcf,'Type','uitoolbar');

% Отключение кнопки Pan и оставление только Zoom
% Поиск кнопки Pan
hpan = findall(htoolbar,'Tag','Exploration.Pan');
if ~isempty(hpan)
    delete(hpan); % Удаляем кнопку Pan
end

% Поиск кнопок Zoom
hzoom_in = findall(htoolbar,'Tag','Exploration.ZoomIn');
hzoom_out = findall(htoolbar,'Tag','Exploration.ZoomOut');

% Убедимся, что кнопки Zoom видимы
if ~isempty(hzoom_in)
    set(hzoom_in, 'Visible', 'on');
end
if ~isempty(hzoom_out)
    set(hzoom_out, 'Visible', 'on');
end

% Построение данных акселерометра на верхней половине
ax1 = subplot(2,1,1);
plot(t, acc_data);
xlabel('Время, сек');
ylabel('Показания акселерометра');
legend(['acc\_x (RMS = ' num2str(rms_acc(1)) ')'], ['acc\_y (RMS = ' num2str(rms_acc(2)) ')'], ['acc\_z (RMS = ' num2str(rms_acc(3)) ')']);
title('Данные акселерометра');
grid on;

% Построение данных гироскопа на нижней половине
ax2 = subplot(2,1,2);
plot(t, gyro_data);
xlabel('Время, сек');
ylabel('Показания гироскопа');
legend(['gyro\_x (RMS = ' num2str(rms_gyro(1)) ')'], ['gyro\_y (RMS = ' num2str(rms_gyro(2)) ')'], ['gyro\_z (RMS = ' num2str(rms_gyro(3)) ')']);
title('Данные гироскопа');
grid on;

% Синхронизация оси X двух графиков
linkaxes([ax1, ax2], 'x');

% Включение режима Zoom по умолчанию и установка зума только по горизонтали
h = zoom(gcf);
set(h,'Motion','horizontal');
set(h,'Enable','on');

% Создание единого объекта timeseries, содержащего данные акселерометра и гироскопа
% Объединение данных в одну матрицу
combined_data = [acc_data, gyro_data]; % размерность: (num_records - ignore_samples) x 6
% Порядок столбцов: [acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z]

% Создание объекта timeseries
sensor_timeseries = timeseries(combined_data, t, 'Name', 'Sensor Data');
sensor_timeseries.DataInfo.Units = 'Units'; % Укажите единицы измерения, если известно
sensor_timeseries.TimeInfo.Units = 'seconds';

% Создание переменной с общим временем записи
total_time = t(end);

% Теперь вы можете использовать 'sensor_timeseries' в Simulink с помощью блока From Workspace
