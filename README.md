## qpdf_transform 

Утилита преобразует входной PDF используя список пар [страница, вращение].
Для примера использования нужно запустить утилиту без аргументов.
Для дополнительной информации можно заглянуть в директорию `docs`.

## Сборка

Перед сборкой стоит запустить `scripts/check.sh`.
Он может указать на потенциальные проблемы.

`make` должно быть достаточно. 
Исполняемый файл - `./bin/qpdf_transform`.

Makefile предполагает, что заголовочные файлы от libqpdf лежат в
`/usr/local/include/qpdf/` или `/usr/share/include/qpdf/`.
Если это не так, то требуется отредактировать его,
либо, как вариант, положить заголовочные файлы в include директорию
в корне репозитория.

## RPM / Установка

Чтобы создать rpm пакет, требуется запустить скрипт `scripts/make_rpm.sh`
Скрипт следует запускать из корня репозитория.
Конфигурация скрипта в самом скрипте (нужно редактировать переменные).

## Возможные проблемы при сборке/установке

-- Предупреждения при включении заголовочных файлов libqpdf  
- Их стоит игнорировать  

-- `ld: cannot find -lqpdf`  
- Тут три варианта. 
  1. Положить so файл в `lib` директорию, перезапустить генерацию пакета и пересобрать его.
  2. Задать путь к so файлу через `LD_LIBRARY_PATH` (не особо надёжный способ).
  3. Установить библиотеку по-нормальному, чтобы `ld` её видел.

-- `Permission denied` при установке пакета (install)  
- Запустить установку пакета из-под пользователя с правами записи в эту директорию

## Коды возврата утилиты

```
0 = Success
1 = InvalidArgCount: "<in-pdf> or <out-pdf> or <pages> arg is missing"
2 = InputFileNotExist: "<in-pdf> file not found (not exists or can not be accessed)"
3 = InputAndOutputOverlap: "<in-pdf> should not be the same as <out-pdf>"
4 = InvalidRotationArg: "invalid rotation value given"
5 = UnexpectedException: "unexpected exception during execution"
6 = BadPageNumber: "bad page number (negative or higher than max page number)"
```
Если qpdf бросает исключение, утилита выходит с кодом `6`.
Ошибка печатается в `stderr`.
