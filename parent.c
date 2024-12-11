#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

void to_uppercase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

void replace_spaces_with_underscores(char *str) {
    while (*str) {
        if (*str == ' ') {
            *str = '_';
        }
        str++;
    }
}

int main() {
    HANDLE hPipe1Read, hPipe1Write, hPipe2Read, hPipe2Write;
    SECURITY_ATTRIBUTES saAttr;
    char buffer[BUFFER_SIZE];
    DWORD bytesRead, bytesWritten;
    STARTUPINFO siChild1, siChild2;
    PROCESS_INFORMATION piChild1, piChild2;
    
    // Создание именованных каналов
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Создаем каналы для передачи данных
    if (!CreatePipe(&hPipe1Read, &hPipe1Write, &saAttr, 0) ||
        !CreatePipe(&hPipe2Read, &hPipe2Write, &saAttr, 0)) {
        printf("CreatePipe failed\n");
        return 1;
    }

    // Создаем процесс child1
    ZeroMemory(&siChild1, sizeof(STARTUPINFO));
    ZeroMemory(&piChild1, sizeof(PROCESS_INFORMATION));
    siChild1.cb = sizeof(STARTUPINFO);

    if (!CreateProcess(NULL, "child1.exe", NULL, NULL, TRUE, 0, NULL, NULL, &siChild1, &piChild1)) {
        printf("CreateProcess failed for child1\n");
        return 1;
    }

    // Создаем процесс child2
    ZeroMemory(&siChild2, sizeof(STARTUPINFO));
    ZeroMemory(&piChild2, sizeof(PROCESS_INFORMATION));
    siChild2.cb = sizeof(STARTUPINFO);

    if (!CreateProcess(NULL, "child2.exe", NULL, NULL, TRUE, 0, NULL, NULL, &siChild2, &piChild2)) {
        printf("CreateProcess failed for child2\n");
        return 1;
    }

    // Родительский процесс читает строки от пользователя и отправляет их в pipe1
    while (1) {
        printf("Enter a string: ");
        if (!fgets(buffer, BUFFER_SIZE, stdin)) break;
        buffer[strcspn(buffer, "\n")] = 0;  // Убираем символ новой строки

        // Отправляем данные в pipe1
        WriteFile(hPipe1Write, buffer, strlen(buffer) + 1, &bytesWritten, NULL);

        // Чтение результата из pipe2
        if (ReadFile(hPipe2Read, buffer, BUFFER_SIZE, &bytesRead, NULL)) {
            printf("Result: %s\n", buffer);
        }
    }

    // Закрытие всех дескрипторов
    CloseHandle(hPipe1Read);
    CloseHandle(hPipe1Write);
    CloseHandle(hPipe2Read);
    CloseHandle(hPipe2Write);

    // Ожидание завершения процессов
    WaitForSingleObject(piChild1.hProcess, INFINITE);
    WaitForSingleObject(piChild2.hProcess, INFINITE);

    return 0;
}