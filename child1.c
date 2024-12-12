#include <stdio.h>
#include <ctype.h>

void convert_to_uppercase(FILE *input, FILE *output) {
    char ch;
    while ((ch = fgetc(input)) != EOF) {
        fputc(toupper(ch), output); 
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        return 2;
    }

    FILE *output = fopen(argv[2], "w");
    if (output == NULL) {
        fclose(input);
        return 3;
    }

    convert_to_uppercase(input, output);

    fclose(input);
    fclose(output);

    return 0;
}
