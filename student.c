#include "student.h"
#include <fcntl.h>    
#include <unistd.h>  
#include <sys/stat.h> 
#include <dirent.h>

Node* studentList = NULL;
Stack undoStack;
Stack redoStack;
Queue pendingDeletions;

void initStack(Stack* s) { s->top = -1; }
int isStackEmpty(Stack* s) { return s->top == -1; }
int isStackFull(Stack* s) { return s->top == MAX - 1; }

void pushStack(Stack* s, Student* student) {
    if (!isStackFull(s)) {
        Student* newStudent = (Student*)malloc(sizeof(Student));
        *newStudent = *student;
        s->items[++s->top] = newStudent;
    }
}

Student* popStack(Stack* s) {
    if (!isStackEmpty(s)) return s->items[s->top--];
    return NULL;
}

void clearStack(Stack* s) {
    while (!isStackEmpty(s)) free(popStack(s));
}

void initQueue(Queue* q) { q->front = 0; q->rear = -1; q->size = 0; }
int isQueueEmpty(Queue* q) { return q->size == 0; }
int isQueueFull(Queue* q) { return q->size == MAX; }

void enqueue(Queue* q, Student* student) {
    if (!isQueueFull(q)) {
        Student* newStudent = (Student*)malloc(sizeof(Student));
        *newStudent = *student;
        q->rear = (q->rear + 1) % MAX;
        q->items[q->rear] = newStudent;
        q->size++;
    }
}

Student* dequeue(Queue* q) {
    if (!isQueueEmpty(q)) {
        Student* student = q->items[q->front];
        q->front = (q->front + 1) % MAX;
        q->size--;
        return student;
    }
    return NULL;
}

void clearQueue(Queue* q) {
    while (!isQueueEmpty(q)) free(dequeue(q));
}

// функция сохранения через системные вызовы Linux
void saves(void) {
    // Открываем файловый дескриптор с правами 0644
    int fd = open("data.bin", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd != -1) {
        Node* current = studentList;
        while (current != NULL) {
            // Системный вызов записи низкого уровня
            write(fd, &(current->data), sizeof(Student));
            current = current->next;
        }
        close(fd); // Закрываем дескриптор
    }
}

// функция загрузки через дескрипторы и lseek
void load(void) {
    int fd = open("data.bin", O_RDONLY);
    if (fd == -1) {
        studentList = NULL;
        return;
    }

    freem();
    Student temp;

    // Читаем последовательно структуры из дескриптора
    while (read(fd, &temp, sizeof(Student)) == sizeof(Student)) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode->data = temp;
        newNode->next = NULL;

        if (studentList == NULL) {
            studentList = newNode;
        }
        else {
            Node* curr = studentList;
            while (curr->next != NULL) curr = curr->next;
            curr->next = newNode;
        }
    }
    close(fd);
}

// Снапшот для Undo 
void saveToUndo(void) {
    if (studentList != NULL) {
        pushStack(&undoStack, &(studentList->data));
    }
    clearStack(&redoStack);
    initStack(&redoStack);
}

void undo(void) {
    if (!isStackEmpty(&undoStack)) {
        if (studentList != NULL) {
            pushStack(&redoStack, &(studentList->data));
        }
        Student* prev = popStack(&undoStack);
        if (studentList != NULL) {
            studentList->data = *prev;
        }
        free(prev);
        saves();
        printf("operation cancel\n");
    }
    else {
        printf("none cancel\n");
    }
}

void redo(void) {
    if (!isStackEmpty(&redoStack)) {
        Student* next = popStack(&redoStack);
        if (studentList != NULL) {
            studentList->data = *next;
        }
        free(next);
        saves();
        printf("repeat\n");
    }
    else {
        printf("no repeat\n");
    }
}

void addToDeletionQueue(int id) {
    Node* curr = studentList;
    while (curr != NULL) {
        if (curr->data.id == id) {
            enqueue(&pendingDeletions, &(curr->data));
            printf("student add on queue delete\n");
            return;
        }
        curr = curr->next;
    }
    printf("Student not found\n");
}

void processDeletionQueue(void) {
    if (isQueueEmpty(&pendingDeletions)) {
        printf("queue is empty\n");
        return;
    }

    while (!isQueueEmpty(&pendingDeletions)) {
        Student* toDelete = dequeue(&pendingDeletions);
        printf("- delete student: %s (ID: %d)\n", toDelete->name, toDelete->id);

        Node* curr = studentList;
        Node* prev = NULL;

        while (curr != NULL) {
            if (curr->data.id == toDelete->id) {
                if (prev == NULL) {
                    studentList = curr->next;
                }
                else {
                    prev->next = curr->next;
                }
                free(curr);
                break;
            }
            prev = curr;
            curr = curr->next;
        }
        free(toDelete);
    }
    saves();
}

void add(void) {
    saveToUndo();
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->next = NULL;

    printf("ID: "); scanf("%d", &newNode->data.id);
    printf("Name: "); scanf("%s", newNode->data.name);
    printf("Score: "); scanf("%f", &newNode->data.score);

    if (studentList == NULL) {
        studentList = newNode;
    }
    else {
        Node* curr = studentList;
        while (curr->next != NULL) curr = curr->next;
        curr->next = newNode;
    }
    saves();
}

void view(void) {
    printf("\n%-10s %-30s %-10s\n", "ID", "Name", "Score");
    Node* curr = studentList;
    while (curr != NULL) {
        printf("%%-10d %-30s %-10.2f\n", curr->data.id, curr->data.name, curr->data.score);
        curr = curr->next;
    }
}

void search(void) {
    int id, found = 0;
    printf("Search ID: "); scanf("%d", &id);
    Node* curr = studentList;
    while (curr != NULL) {
        if (curr->data.id == id) {
            printf("%d %s %.2f\n", curr->data.id, curr->data.name, curr->data.score);
            found = 1;
            break;
        }
        curr = curr->next;
    }
    if (!found) printf("Not found\n");
}

void update(void) {
    int id, found = 0;
    printf("Update ID: "); scanf("%d", &id);
    Node* curr = studentList;
    while (curr != NULL) {
        if (curr->data.id == id) {
            saveToUndo();
            printf("New name: "); scanf("%s", curr->data.name);
            printf("New score: "); scanf("%f", &curr->data.score);
            found = 1;
            break;
        }
        curr = curr->next;
    }
    if (found) saves();
}

void deleted(void) {
    int id;
    printf("Delete ID: "); scanf("%d", &id);
    printf("1 - delete \n2 - add queue delete\n");
    int choice; scanf("%d", &choice);

    if (choice == 2) {
        addToDeletionQueue(id);
        return;
    }

    saveToUndo();
    Node* curr = studentList;
    Node* prev = NULL;
    while (curr != NULL) {
        if (curr->data.id == id) {
            if (prev == NULL) studentList = curr->next;
            else prev->next = curr->next;
            free(curr);
            saves();
            break;
        }
        prev = curr;
        curr = curr->next;
    }
}

// Сортировка связного списка методом пузырька 
void sortById(void) {
    saveToUndo();
    if (studentList == NULL) return;
    int swapped;
    Node* ptr1;
    Node* lptr = NULL;
    do {
        swapped = 0;
        ptr1 = studentList;
        while (ptr1->next != lptr) {
            if (ptr1->data.id > ptr1->next->data.id) {
                Student temp = ptr1->data;
                ptr1->data = ptr1->next->data;
                ptr1->next->data = temp;
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    saves();
}

void showMenu(void) {
    printf("\n1 - add student\n2 - view students\n3 - found student\n4 - update\n5 - delete\n");
    printf("6 - sorted ID\n7 - cancel (Undo)\n8 - repeat (Redo)\n9 - process queue\n10 - recursion find backup\n11 - exit\nur choise: ");
}

void freem(void) {
    Node* curr = studentList;
    while (curr != NULL) {
        Node* next = curr->next;
        free(curr);
        curr = next;
    }
    studentList = NULL;
    clearStack(&undoStack);
    clearStack(&redoStack);
    clearQueue(&pendingDeletions);
}
// Рекурсивное итерирование элементов каталога
void findBackupFiles(const char* dirPath) {
    DIR* dir = opendir(dirPath);

    struct dirent* entry;
    char path[1024];

    // Итерируем элементы каталога
    while ((entry = readdir(dir)) != NULL) {
        // защита от бесконечной рекурсии
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Формируем полный путь к элементу
        snprintf(path, sizeof(path), "%s/%s", dirPath, entry->d_name);

        // Если это каталог — уходим в рекурсию
        if (entry->d_type == DT_DIR) {
            findBackupFiles(path);
        }
        // Если это обычный файл — проверяем расширение
        else if (entry->d_type == DT_REG) {
            if (strstr(entry->d_name, ".bin") != NULL) {
                printf("[Найдена резервная копия]: %s\n", path);
            }
        }
    }
    closedir(dir);
}
// Добавляем пункт в интерфейс вызова
void searchBackups(void) {
    char dirName[256];
    printf("path: ");
    scanf("%s", dirName);
    findBackupFiles(dirName);
}