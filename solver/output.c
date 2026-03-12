
#include "output.h"

FILE* outputFile() {
    return outFile;
}

void output(const char *fileName, Field F) {
    int i, j;
    FILE *file = fopen(fileName, "w");
    if (file == NULL) {
        perror("File open error");
        return;
    }
    debugOut("W: %d, H: %d\n", F.width, F.height);
    //fprintf(file, "W: %d, H: %d\n", F.width, F.height);
    for (i = 0; i < F.height; i++) {
        for (j = 0; j < F.width; j++) {
            debugOut("%c", F.map[i][j]);
            //fprintf(file, "%c", F.map[i][j]);
        }
        debugOut("\n");
        //fprintf(file, "\n");

    }
    fclose(file);
}

void print2DArrayToFile(char *fileName, char **array, int height, int width) {
    int i, j;
    FILE *file = fopen(fileName, "w");
    if (file == NULL) {
        perror("File open error");
        return;
    }

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            fprintf(file, "%c ", array[i][j]);
            printf("%c", array[i][j]);

        }
        fprintf(file, "\n");
        printf("\n");
    }
    fclose(file);
}

void debugOut ( const char * format, ... ) { 
    if (!isDebug){
        return;
    }
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void out ( const char * format, ... ) {
    FILE* of = outputFile();
    va_list args;
    va_start(args, format);
    if (isDebug){
        vprintf(format, args);
    }
    if (of != NULL) {
        vfprintf(of, format, args);
    }
    va_end(args);
}

