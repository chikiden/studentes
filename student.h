#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define MAX 1000

// Макрос
#if defined(_WIN32)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

typedef struct {
    int id;
    char name[50];
    float score;
} Student;


typedef struct Node {
    Student data;
    struct Node* next;
} Node;

// Стек
typedef struct {
    Student* items[MAX];
    int top;
} Stack;

// Очередь
typedef struct {
    Student* items[MAX];
    int front;
    int rear;
    int size;
} Queue;

// Глобальные переменные 
extern Node* studentList;
extern Stack undoStack;
extern Stack redoStack;
extern Queue pendingDeletions;

// Интерфейс функций 
EXPORT void initStack(Stack* s);
EXPORT int isStackEmpty(Stack* s);
EXPORT int isStackFull(Stack* s);
EXPORT void pushStack(Stack* s, Student* student);
EXPORT Student* popStack(Stack* s);
EXPORT void clearStack(Stack* s);

EXPORT void initQueue(Queue* q);
EXPORT int isQueueEmpty(Queue* q);
EXPORT int isQueueFull(Queue* q);
EXPORT void enqueue(Queue* q, Student* student);
EXPORT Student* dequeue(Queue* q);
EXPORT void clearQueue(Queue* q);

EXPORT void saves(void);
EXPORT void load(void);
EXPORT void saveToUndo(void);
EXPORT void undo(void);
EXPORT void redo(void);

EXPORT void addToDeletionQueue(int id);
EXPORT void processDeletionQueue(void);

EXPORT void add(void);
EXPORT void view(void);
EXPORT void search(void);
EXPORT void update(void);
EXPORT void deleted(void);
EXPORT void sortById(void);
EXPORT void freem(void);
EXPORT void showMenu(void);
EXPORT void findBackupFiles(const char* dirPath);
EXPORT void searchBackups(void);
#endif
