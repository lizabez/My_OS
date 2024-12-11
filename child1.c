#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

int main() {
    HANDLE hPipe1Read, hPipe2Write;
    char buffer[BUFFER_SIZE];
    DWORD bytesRead, bytesWritten;

    // Получение дескрипторов pipe от родительского процесса
    hPipe1Read = GetStdHandle(STD_INPUT_HANDLE);
    hPipe2Write = GetStdHandle(STD_OUTPUT_HANDLE);

    while (1) {
        if (ReadFile(hPipe1Read, buffer, BUFFER_SIZE, &bytesRead, NULL) == FALSE) break;

        // Переводим строку в верхний регистр
        for (int i = 0; i < bytesRead; i++) {
            buffer[i] = toupper((unsigned char)buffer[i]);
        }

        // Отправляем данные в следующий процесс
        WriteFile(hPipe2Write, buffer, bytesRead, &bytesWritten, NULL);
    }

    return 0;
}
