#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "field.h"

int isDebug;
FILE* outFile;

void print2DArrayToFile(char *fileName, char **array, int height, int width);
void output(const char *fileName, Field F);
void debugOut ( const char * format, ... );
void out ( const char * format, ... );