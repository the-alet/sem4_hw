#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "field.h"


FinalPosition input(const char *fileName, Field *field) {
    FILE *file = fopen(fileName, "r");
    assert(file != NULL);

    int cols, rows, i, numOfTargetFigures;
    fscanf(file, "%d %d", &rows, &cols);
    fgetc(file);

    initField(field, cols, rows);

    for (i = 0; i < rows; i++) {
        fgets(field->map[i], cols + 1, file);
        fgetc(file);
    }
    fscanf(file, "%d", &numOfTargetFigures);
    FinalPosition pos;
    fscanf(file, "%d %d %c", &pos.row, &pos.col, &pos.figure);
    field->index = -1;
    
    fclose(file);
    return pos;
}

