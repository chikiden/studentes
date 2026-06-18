#include "student.h"

#include <locale.h>

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    // Обработка параметров командной строки
    if (argc > 1) {
        if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
            printf("Управление студентами v3.0 (КТ4 + КТ5 + КТ6)\n");
            printf("Разработчик: chikiden\n");
            return 0; // Завершаем работу, если просили только версию
        }
        else if (strcmp(argv[1], "-l") == 0) {
            printf("Режим отладки активирован.\n");
        }
    }

    initStack(&undoStack);
    initStack(&redoStack);
    initQueue(&pendingDeletions);

    load();

    typedef void (*MenuFunction)();
    MenuFunction menu[] = { NULL, add, view, search, update, deleted, sortById, undo, redo, processDeletionQueue, searchBackups };

    int choice;
    while (1) {
        showMenu();
        if (scanf("%d", &choice) != 1) break;

        if (choice == 11) {
            freem();
            break;
        }

        if (choice >= 1 && choice <= 10 && menu[choice] != NULL) {
            menu[choice]();
        }
    }

    return 0;
}
}
