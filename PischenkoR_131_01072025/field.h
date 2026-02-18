#pragma once


typedef struct {
    int row;
    int col;
    char figure;
} FinalPosition;

typedef struct Field {
    int index;
    int width, height;
    char **map;
} Field;

void initField(Field *field, int width, int height);
void deleteField(Field *field);