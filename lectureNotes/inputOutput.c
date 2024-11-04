#include <stdio.h>

int main() {
    FILE *inputFile = fopen("file.txt", "r");
    FILE *outputFile = fopen("fileOut.txt", "w");

    if (inputFile ==NULL || outputFile == NULL) {
        printf("Error");
        return 1;
    }

    double number;

    while (fscanf(inputFile, "%lf", &number) == 1) {
        printf("%.3lf ", number * 2);
        fprintf(outputFile, "%.3lf", number * 2); //Write multiplied number to output file
        
        char ch = fgetc(inputFile); //Read next character
        if (ch == '\n' || ch==EOF) {
            printf("\n");
            fprintf(outputFile, "\n"); //Write new line to output after each line
        }
    }
    fclose(inputFile);
    fclose(outputFile);
}