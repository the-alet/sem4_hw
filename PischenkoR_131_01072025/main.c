#include "input.h"
#include "search.h"
#include "output.h"

int isDebug = 0;
FILE* outFile;

int main(int argc, char ** argv[]) {
    Field startPos;
    FinalPosition position;
    const char* oF = argc == 3 ? (const char*)argv[2] : "./result.txt";
    outFile = fopen(oF, "w");
    position = input((const char *)argv[1], &startPos);
    if(!search(&startPos, &position)) {
        out("\nWay not found!!!");
    }
    deleteField(&startPos);
    fclose(outFile);
    return 0;
}