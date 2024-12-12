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
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Error opening input file");
        return 1;
    }

    FILE *output = fopen(argv[2], "w");
    if (output == NULL) {
        perror("Error opening output file");
        fclose(input);
        return 1;
    }

    convert_to_uppercase(input, output);

    fclose(input);
    fclose(output);

    return 0;
}
