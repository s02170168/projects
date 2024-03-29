# Task5
Задание shell.
## Синтаксис реализованного интерпретатора в виде БНФ
<команда_shell> ::= <список_команд>  
<список_команд> ::= <конвейер> { [один из &, ;] <конвейер> } [ &, ; ]  
<конвейер> ::= <команда> { [один из |, ||, && ] <команда> }  
<команда> ::= <простая команда> [один из |, ||, && ] (<список_команд)  
[< <имя_файла> ] [ [один из >, >>] <имя_файла> ]  
<простая_команда> ::= <имя_файла> {<аргумент>} [< <имя_файла>] [ [один из >, >>] <имя_файла> ]

## Особенности реализации
При запуске без аргументов программа работает как интерпретатор.  

Добавлена поддержка скобок в командах интерпретатора.

./my_shell arg1 .. argN исполнит запустит my_shell и исполнит команды arg1 .. argN.

## Сборка проекта
Использована утилита make. Добавлена директива clean.
## Список файлов проекта:
* main.c
>Содержит объявление переменных для работы с буфером, списком и набором команд.  
>Построчно считывает символы и переводит их в список слов.
>Превращает список слов в команды и передаёт их на запуск.
>Обрабатывает получаемые ошибки.
* buf.h
>Объявляет структуру buf, необходимую для накопления символов и заполнения списка слов.  
>Также объявлены функции очищения, создания удаления буфера, добавления в буфер символа,  
>добавления строки в список слов и функция, которая анализирует и добавляет полученный символ.
* buf.c
>Реализация функций buf.h.
* list.h
>Объявление структуры list, содержащей массив строк и размер этого массива.
>Здесь объявлены функции, обрабатывающие спикок слов.
* list.c
>Реализация функций list.h.
* cmd.h
>Объявление структур command и cmd(список команд).  
>Реализовано заполнение структуры cmd с помощью структуры list.  
>Также здесь находятся функции, реализующие запуск команд интерпретатора.
* cmd.c
>Реализацию функций cmd.h.
* symbols.h
>Библиотека обрабатываемых символов и функции для их обработки.
* symbols.c
>Реализация функций symbols.h.
* error.h
>Объявление типа возникающих ошибок и функции, которая о них сообщает.
* error.c
>Реализация error.h.
* myShellHeaders.h
>Включены все необходимые стандартные библиотеки для работы с процессами,
>а также error.h и symbols.h.
* Makefile

## Тест Markdown

```c
int main(void){
    printf("Hello, world!\n");
}
```

```asm
CMAIN:
    mov ebp, esp
    push ebp

    mov eax, 1
.cycle:
    mov ebx, 2
    mul ebx
    jnc .cycle

    pop ebp
    mov esp, ebp
    ret
```