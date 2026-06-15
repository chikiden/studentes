CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET_STATIC = app_static
TARGET_SHARED = app_shared
TARGET_RUNTIME = app_runtime

all: static dynamic runtime

# Сборка со статической библиотекой
static: student.c student.h main.c
	$(CC) $(CFLAGS) -c student.c -o student.o
	ar rcs libstudent.a student.o
	$(CC) $(CFLAGS) main.c -L. -lstudent -o $(TARGET_STATIC)

# Сборка с динамической библиотекой
dynamic: student.c student.h main.c
	$(CC) $(CFLAGS) -fPIC -c student.c -o student_shared.o
	$(CC) -shared -o libstudent.so student_shared.o
	$(CC) $(CFLAGS) main.c -L. -lstudent -Wl,-rpath,. -o $(TARGET_SHARED)

# Линковка времени выполнения через dlopen
runtime: student.c student.h main_dynamic.c dynamic
	$(CC) $(CFLAGS) -rdynamic main_dynamic.c -ldl -o $(TARGET_RUNTIME)

clean:
	rm -f *.o *.a *.so $(TARGET_STATIC) $(TARGET_SHARED) $(TARGET_RUNTIME)
