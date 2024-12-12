#include <stdio.h>

void replace_spaces_with_underscore(FILE *input, FILE *output) {
    char ch;
    while ((ch = fgetc(input)) != EOF) {
        if (ch == ' ') {
            fputc('_', output); 
        } else {
            fputc(ch, output);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        return 5;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        return 6;
    }

    FILE *output = fopen(argv[2], "w");
    if (output == NULL) {
        fclose(input);
        return 7;
    }

    replace_spaces_with_underscore(input, output);

    fclose(input);
    fclose(output);

    return 0;
}
