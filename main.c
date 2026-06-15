#include "student.h"

#include <locale.h>

int main(void) {

    setlocale(LC_ALL, "ru_RU.UTF-8");



    initStack(&undoStack);

    initStack(&redoStack);

    initQueue(&pendingDeletions);



    load();



    typedef void (*MenuFunction)();

    MenuFunction menu[] = { NULL, add, view, search, update, deleted, sortById, undo, redo, processDeletionQueue, searchBackups };



    int choice;

    while (1) {

        showMenu();

        scanf("%d", &choice);



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
