#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <locale.h>
#include "student.h"

int main(void) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    // Загружаем разделяемый объект времени выполнения
    void* handle = dlopen("./libstudent.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
        return 1;
    }

    // Динамически связываем функции через указатели
    void (*initStack_fn)(Stack*) = dlsym(handle, "initStack");
    void (*initQueue_fn)(Queue*) = dlsym(handle, "initQueue");
    void (*load_fn)(void) = dlsym(handle, "load");
    void (*showMenu_fn)(void) = dlsym(handle, "showMenu");
    void (*freem_fn)(void) = dlsym(handle, "freem");
    void (*add_fn)(void) = dlsym(handle, "add");
    void (*view_fn)(void) = dlsym(handle, "view");
    void (*search_fn)(void) = dlsym(handle, "search");
    void (*update_fn)(void) = dlsym(handle, "update");
    void (*deleted_fn)(void) = dlsym(handle, "deleted");
    void (*sortById_fn)(void) = dlsym(handle, "sortById");
    void (*undo_fn)(void) = dlsym(handle, "undo");
    void (*redo_fn)(void) = dlsym(handle, "redo");
    void (*processDeletionQueue_fn)(void) = dlsym(handle, "processDeletionQueue");
    
    // Динамически загружаем функцию поиска бэкапов
    void (*searchBackups_fn)(void) = dlsym(handle, "searchBackups"); 

    // Получаем указатели на глобальные переменные
    Stack* undoStack_ptr = (Stack*)dlsym(handle, "undoStack");
    Stack* redoStack_ptr = (Stack*)dlsym(handle, "redoStack");
    Queue* pendingDeletions_ptr = (Queue*)dlsym(handle, "pendingDeletions");

    // Инициализируем через полученные указатели (проверяем, что dlsym нашел)
    if (initStack_fn && undoStack_ptr) initStack_fn(undoStack_ptr);
    if (initStack_fn && redoStack_ptr) initStack_fn(redoStack_ptr);
    if (initQueue_fn && pendingDeletions_ptr) initQueue_fn(pendingDeletions_ptr);

    if (load_fn) load_fn();

    typedef void (*MenuFunction)();
    MenuFunction menu[] = { NULL, add_fn, view_fn, search_fn, update_fn, deleted_fn, sortById_fn, undo_fn, redo_fn, processDeletionQueue_fn, searchBackups_fn };

    int choice;
    while (1) {
        if (showMenu_fn) showMenu_fn();
        if (scanf("%d", &choice) != 1) break;


        if (choice == 11) {
            if (freem_fn) freem_fn();
            break;
        }

        if (choice >= 1 && choice <= 10 && menu[choice] != NULL) {
            menu[choice]();
        }
    }

    dlclose(handle);
    return 0;
}

