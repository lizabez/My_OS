#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void run_child(const char *child_exe, const char *input_file, const char *output_file) {
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi;
    
   
    char command[1024];
    snprintf(command, sizeof(command), "\"%s\" \"%s\" \"%s\"", child_exe, input_file, output_file);
    

    si.cb = sizeof(si);
    

    if (!CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        fprintf(stderr, "Error starting process %s\n", child_exe);
        exit(1);
    }
    
    WaitForSingleObject(pi.hProcess, INFINITE);
    

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

int main() {
    const char *input_file = "input.txt";
    const char *child1_output = "intermediate.txt";
    const char *child2_output = "output.txt";

    // Запуск child1
    printf("Запуск child1...\n");
    run_child("child1.exe", input_file, child1_output);
    
    // Запуск child2
    printf("Запуск child2...\n");
    run_child("child2.exe", child1_output, child2_output);

    printf("Преобразования завершены. Результат в файле %s\n", child2_output);

    return 0;
}
