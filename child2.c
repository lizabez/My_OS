#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BUFFER_SIZE 1024

int main() {
    HANDLE hPipe2Read;
    char buffer[BUFFER_SIZE];
    DWORD bytesRead;

    // Получение дескриптора pipe
    hPipe2Read = GetStdHandle(STD_INPUT_HANDLE);

    while (1) {
        if (ReadFile(hPipe2Read, buffer, BUFFER_SIZE, &bytesRead, NULL) == FALSE) break;

        // Заменяем пробелы на подчеркивания
        for (int i = 0; i < bytesRead; i++) {
            if (buffer[i] == ' ') {
                buffer[i] = '_';
            }
        }

        // Выводим результат
        printf("%s\n", buffer);
    }

    return 0;
}
