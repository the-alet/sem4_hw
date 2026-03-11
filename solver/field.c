#include <stdlib.h>
#include <assert.h>

#include "field.h"

void initField(Field *field, int w, int h) {
    int i;

    assert(field != NULL);

    field->width = w;
    field->height = h;
    field->map = malloc(sizeof(char *) * h);
    assert(field->map != NULL);
    for (i = 0; i < h; i++) {
        field->map[i] = malloc(sizeof(char) * w + 1);
        assert(field->map[i] != NULL);
    }

    field->index = 0;
}

void deleteField(Field *field){
    int i;
    for (i = 0; i < field->height; i++) {
        free(field->map[i]);
    }
    free(field->map);
}