#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "output.h"
#include "search.h"

#define MAX_LEN 100000

enum Direction { dirUp = 1, dirRight = 2, dirDown = 3, dirLeft = 4};

typedef struct Node {
    char key[128];
    Node *left, *right, *prev;
} Node;

typedef struct Item {
    State *data;
    Item *next;
} Item;

typedef struct FiguresDictionary {
    int *width;
    int *height;
    int currLen;
} FiguresDictionary;

typedef struct Figure{
    int id, x, y;
} Figure;

typedef struct State {
    Figure *array;
    enum Direction moveDir;
    int figureMoveIndex;
    int currLen;
    Node *node;
} State;

typedef struct Queue{
    Item *first, *last;
} Queue;

typedef struct History{
    Node *root;
} History;

typedef struct Context 
{
    Queue queue;
    History history;
    State *buffer;
    Field startPosition;
    State startState;
    FinalPosition finalPosition;
    FiguresDictionary figDict;
} Context;

int getFigureWidth(const Field *field, char figure, int x, int y){
    int fWidth = 1;
    
    if (figure == '#') {
        return 1;
    }

    if (x < field->width - 1) {
        for (; x + fWidth < field->width && field->map[y][x + fWidth] == figure; fWidth++){}
    }
    return fWidth;
}

int getFigureHeight(const Field *field, char figure, int x, int y){
    int fHeight = 1;

    if (figure == '#') {
        return 1;
    }

    if (y < field->height - 1) {
        for (; y + fHeight < field->height && field->map[y + fHeight][x] == figure; fHeight++){}
    }
    return fHeight;
}

int isInCharArr(const char *arr, int len, char symbol) {
    int i;

    for (i = 0; i < len; i++) {
        if (arr[i] == symbol)
            return 1;
    }
    return 0;
}

int isInIntArr(const int *arr, int len, int number) {
    int i;

    for (i = 0; i < len; i++) {
        if (arr[i] == number)
            return 1;
    }
    return 0;
}

int isInDictionary(const FiguresDictionary *figDict, int w, int h) {
    int i;

    for (i = 2; i < figDict->currLen; i++) {
        if (figDict->width[i] == w && figDict->height[i] == h) {
            return 1;
        }
    }
    return 0;
}

void initDictionary(FiguresDictionary *figDict, const Field *startPos, const FinalPosition *finalPos) {
    int maxCnt = startPos->width * startPos->height;
    char *inDict = malloc(maxCnt);
    int i, j, x = 0, y = 0, flag = 0, figuresCount = 0;
    assert(inDict != NULL);
    
    figDict->width = malloc(maxCnt * sizeof(int));
    figDict->height = malloc(maxCnt * sizeof(int));

    figDict->width[0] = 1;
    figDict->height[0] = 1;
    inDict[figuresCount++] = '#';

    // find top left corner of target figure
    for (i = 0; i < startPos->height; i++) {
        if (flag) {
            break;
        }
        for (j = 0; j < startPos->width; j++) {
            if (startPos->map[i][j] == finalPos->figure) {
                x = j;
                y = i;
                flag = 1;
                break;
            }
        }
    }

    // find width and height of target figure
    figDict->width[1] = getFigureWidth(startPos, finalPos->figure, x, y);
    figDict->height[1] = getFigureHeight(startPos, finalPos->figure, x, y);
    inDict[figuresCount++] = finalPos->figure;

    // collect other figures
    figDict->currLen = 2;

    for (j = 0; j < startPos->width; j++) {
        for (i = 0; i < startPos->height; i++) {
            if(!isInCharArr(inDict, maxCnt, startPos->map[i][j])) {
                int fWidth = getFigureWidth(startPos, startPos->map[i][j], j, i);
                int fHeight = getFigureHeight(startPos, startPos->map[i][j], j, i);
                if (!isInDictionary(figDict, fWidth, fHeight)) {
                    int len = figDict->currLen;
                    figDict->width[len] = fWidth;
                    figDict->height[len] = fHeight;
                    figDict->currLen++;
                }
                inDict[figuresCount] = startPos->map[i][j];
                figuresCount++;
            } 
        }
    }
    debugOut("\nFigures dictionary\n");
    for (i = 0; i < figDict->currLen; i++) {
        debugOut("%d %d %d\n", i, figDict->width[i], figDict->height[i]);
    }
    free(inDict);
}

int getDictionaryIndex(const FiguresDictionary *figDict, int width, int height) {
    int i;

    for (i = 2; i < figDict->currLen; i++) {
        if (figDict->width[i] == width && figDict->height[i] == height) {
            return i;
        }
    }

    return -1;
}

void printField(const Field *field) {
    int i, j;
    out("\n");

    for (i = 0; i < field->height; i++) {
        for (j = 0; j < field->width; j++) {
            out("%c", field->map[i][j]);
        }
        out("\n");
    }
    out("\n");
}

void printState(const State *state, const Field *field){
    int i, j;
    char **map;

    map = malloc(field->height * sizeof(char *)); 
    assert(map != NULL);
    for (i = 0; i < field->height; i++){
        map[i] = malloc(field->width * sizeof(char));
        assert(map[i] != NULL);
        memset(map[i], '*', field->width * sizeof(char));
    }
    for (i = 0; i < state->currLen; i++) {
        int x = state->array[i].x;
        int y = state->array[i].y;

        map[y][x] = '0' + state->array[i].id;
    }
    out("\n");

    for (i = 0; i < field->height; i++){
        for (j = 0; j < field->width; j++) {
            out("%c", map[i][j]);
        }
        out("\n");
    }
    //for (i = 0; i < state->currLen; i++) {
    //    debugOut("[%d (%d, %d)]", state->array[i].id, state->array[i].x, state->array[i].y);
    //}

    for (i = 0; i < field->height; i++) {
        free(map[i]);
    }

    free(map);

    //out("Previous index: %d\n", state->prevIndex);
}

void toHashData(const State *state, char *hashData) {
    int i;
    for (i = 0; i < 128; i++) {
        hashData[i] = '#';
    }
    hashData[0] = 'a' + state->figureMoveIndex;
    hashData[1] = '0' + state->moveDir;
    for (i = 0; i < state->currLen; i++) {
        hashData[3 * i + 2] = '0' + state->array[i].id;
        hashData[3 * i + 3] = '0' + state->array[i].x;
        hashData[3 * i + 4] = '0' + state->array[i].y;
    }
    return;
}

void initState(State *state, const FiguresDictionary *figDict, const Field *field, const FinalPosition *finalPos) {
    int i, j, stateLen = 0;
    int maxCnt = field->width * field->height;

    char *inState = malloc(maxCnt * sizeof(char));
    assert(inState != NULL);
    memset(inState, 0, maxCnt);
    state->array = malloc(maxCnt * sizeof(Figure));
    state->moveDir = 0;
    state->figureMoveIndex = 0;
    state->currLen = 0;

    for (i = 0; i < field->height; i++) {
        for (j = 0; j < field->width; j++) {
            char symbol = field->map[i][j];
            if (!isInCharArr(inState, maxCnt, symbol) || symbol == '#') {
                int width, height;
                Figure figure;

                width = getFigureWidth(field, symbol, j, i);
                height = getFigureHeight(field, symbol, j, i);
                
                figure.x = j;
                figure.y = i;
                figure.id = symbol == '#' ? 0 : (symbol == finalPos->figure ? 1 : getDictionaryIndex(figDict, width, height));
                state->array[state->currLen++] = figure;
                
                inState[stateLen++] = symbol;
            }
        }
    }

    free(inState);
    //debugOut("\nState\n");
    //printState(state, field);
}

void addToQueue(Queue *queue, const State *value, const Node *node) {

    if (queue->last->next == NULL) {
        Item *curr = queue->last;
        Item *newItem;
        newItem = malloc(sizeof(Item));
        assert(newItem != NULL);
        newItem->next = NULL;
        curr->next = newItem;
    }
    queue->last = queue->last->next;

    queue->last->data = malloc(sizeof(State));
    *queue->last->data = *value;
    queue->last->data->array = malloc(sizeof(Figure) * value->currLen);
    assert(queue->last->data->array != NULL);
    memcpy(queue->last->data->array, value->array, sizeof(Figure) * value->currLen);
    queue->last->data->node = node;
}
void initContext(Context *context, const Field *startPos, const FinalPosition *finalPos) {
    initDictionary(&context->figDict, startPos, finalPos);
    State *startState = malloc(sizeof(State));
    char startStateHash[128];
    initState(startState, &context->figDict, startPos, finalPos);
    context->buffer = malloc(sizeof(State) * MAX_LEN);
    
    Node *root = malloc(sizeof(Node));
    assert(root != NULL);
    context->history.root = root;
    toHashData(startState, startStateHash);
    strcpy(root->key, startStateHash);
    root->left = NULL;
    root->right = NULL;
    root->prev = NULL;
    startState->node = context->history.root;

    Item *newItem;
    newItem = malloc(sizeof(Item));
    assert(newItem != NULL);
    newItem->data = startState;
    newItem->next = NULL;
    context->queue.first = newItem;
    context->queue.last = newItem;
    
    context->startPosition = *startPos;
    context->finalPosition = *finalPos;
    
    context->startState = *startState;
}

// History options

State* fromHashData(const Node *node, const Context *context) {
    int i = 0;

    State *res = malloc(sizeof(State));
    assert(res != NULL);
    res->currLen = context->startState.currLen;
    res->array = malloc(sizeof(Figure) * res->currLen);
    assert(res->array != NULL);
    //initState(res, &context->figDict, &context->startPosition, &context->finalPosition);
    res->figureMoveIndex = node->key[0] - 'a';
    res->moveDir = node->key[1] - '0';
    for (i = 0; i < res->currLen; i++) {
        res->array[i].id = node->key[3 * i + 2] - '0';
        res->array[i].x = node->key[3 * i + 3] - '0';
        res->array[i].y = node->key[3 * i + 4] - '0';
    }
    
    res->node = node;
    return res;

}


int isSameState(const State *state1, const State *state2) {
    //if (state1->currLen != state2->currLen) {
    //    return 0;
    //}
    //int i;
    //// TODO: sort by id>x>y at state creation
    //for (i = 0; i < state1->currLen; i++) {
    //    if (state1->array[i].id != state2->array[i].id ||
    //        state1->array[i].x != state2->array[i].x ||
    //        state1->array[i].y != state2->array[i].y) {
    //        return 0;
    //    }
    //}
    //return 1;

    if (strncmp(state1->node->key + 2, state2->node->key + 2, 128 - 2) == 0) {
        return 1;
    }

    return 0;
}


Node* isInTree(Node *root, const char *key) {
    if (root == NULL) {
        return NULL;
    }

    int cmp = strncmp(key + 2, root->key + 2, 128 - 2);
    
    if (cmp == 0) {
        return root;
    }
    else if (cmp < 0) {
        return isInTree(root->left, key);
    }
    else {
        return isInTree(root->right, key);
    }
}


Node* getHistoryItem(const History *history, const State *state) {
    int i;
    char stateHash[128];
    Node *currNode = history->root;

    toHashData(state, stateHash);

    return isInTree(history->root, stateHash);
}

Node *initNode(History *history, State *state, char *stateHash) {
    Node *newNode = malloc(sizeof(Node));
    assert(newNode != NULL);

    memcpy(newNode->key, stateHash, 128);
    newNode->left = NULL;
    newNode->right = NULL;
    char prevStateHash[128];
    toHashData(state, prevStateHash);
    newNode->prev = getHistoryItem(history, state);

    return newNode;
}

int addToHistory(History *history, const State *state, const Node *prev) {
    if (getHistoryItem(history, state) != NULL) {
        return 0;
    }
    
    Node *currNode = history->root;
    
    char stateHash[128];
    toHashData(state, stateHash);

    while (currNode != NULL) {
        int cmp = strncmp(stateHash + 2, currNode->key + 2, 128 - 2);
        if (cmp < 0) {
            if (currNode->left == NULL) {
                currNode->left = initNode(history, state, stateHash);
                currNode->left->prev = prev;
                return 1;
            }
            currNode = currNode->left;
        }
        else {
            if (currNode->right == NULL) {
                currNode->right = initNode(history, state, stateHash);
                currNode->right->prev = prev;
                return 1;
            }
            currNode = currNode->right;
        }

    }

    return 1;
}

/// to do:: адаптировать код к измененной работе истории и очереди

// Queue options

int isQueueEmpty(const Queue *queue) {
    Item *curr = queue->first;
    if (curr == NULL) {
        return 1;
    }
    if (curr->data == NULL) {
        return 1;
    }

    return 0;
}

State* qPop(Queue *queue) {
    State *data;
    Item *tmp = queue->first;

    data = queue->first->data;
    queue->first = queue->first->next;

    tmp->next = queue->last->next;
    queue->last->next = tmp;
    tmp->data = NULL;

    return data;
}

// search functions


int isEndPosition(const State *state, const FinalPosition position) {
    int i, index;

    for (i = 0; i < state->currLen; i++) {
        if (state->array[i].id == 1) {
            index = i;
        }
    }

    if (state->array[index].x == position.col - 1 && state->array[index].y == position.row - 1) {
        return 1;
    }
    return 0;
}

int findFigure(const State *state, int x, int y, const FiguresDictionary *figDict) {
    int  i, n = 1;

    for (i = 0; i < state->currLen; i++) {
        const int x1 = state->array[i].x;
        const int x2 = x1 + figDict->width[state->array[i].id];
        const int y1 = state->array[i].y;
        const int y2 = y1 + figDict->height[state->array[i].id];

        if (x >= x1 && y >= y1 && x < x2 && y < y2) {
            return i;
        }
    }
    return 0;
}

int canMove(const State *state, enum Direction dir, int index, FiguresDictionary *figDict) {
    Figure fig = state->array[index];
    int i;
    switch(dir) {
    case dirUp:
        for (i = 0; i < figDict->width[fig.id]; i++) {
            const int figureIndex = findFigure(state, fig.x + i, fig.y - 1, figDict);
            if (state->array[figureIndex].id != 0) {
                return 0;
            }
        }
        return 1;
    case dirDown:
        for (i = 0; i < figDict->width[fig.id]; i++) {
            const int figureIndex = findFigure(state, fig.x + i, fig.y + figDict->height[fig.id], figDict);
            if (state->array[figureIndex].id != 0) {
                return 0;
            }
        }
        return 1;
    case dirLeft:
        for (i = 0; i < figDict->height[fig.id]; i++) {
            const int figureIndex = findFigure(state, fig.x - 1, fig.y + i, figDict);
            if (state->array[figureIndex].id != 0) {
                return 0;
            }
        }
        return 1;
    case dirRight:
        for (i = 0; i < figDict->height[fig.id]; i++) {
            const int figureIndex = findFigure(state, fig.x + figDict->width[fig.id], fig.y + i, figDict);
            if (state->array[figureIndex].id != 0) {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

int compareFigures(const Figure *f1, const Figure *f2) {
    if (f1->y > f2->y) {
        return 1;
    }
    if (f1->y < f2->y) {
        return -1;
    }
    if (f1->x > f2->x) {
        return 1;
    }
    if (f1->x < f2->x) {
        return -1;
    }
    return 0;
} 

State move(const State *state, enum Direction dir, int index, FiguresDictionary *figDict) {
    State newState = *state;
    newState.array = malloc(newState.currLen * sizeof(Figure));
    memcpy(newState.array, state->array, newState.currLen * sizeof(Figure));
    Figure fig = state->array[index];
    int i, neighbourIndex, minNIndex, maxNIndex;

    minNIndex = MAX_LEN;
    maxNIndex = -1;
    newState.figureMoveIndex = index;
    
    switch (dir) {
    case dirUp:
        for (i = 0; i < figDict->width[fig.id]; i++) {
            neighbourIndex = findFigure(&newState, fig.x + i, fig.y - 1, figDict);
            minNIndex = min(minNIndex, neighbourIndex);
            maxNIndex = max(maxNIndex, neighbourIndex);
            newState.array[neighbourIndex].y = state->array[index].y + figDict->height[fig.id] - 1;
        }
        newState.moveDir = dirUp;
        newState.array[index].y -= 1;
        break;
    case dirDown:
        for (i = 0; i < figDict->width[fig.id]; i++) {
            neighbourIndex = findFigure(&newState, fig.x + i, fig.y + figDict->height[fig.id], figDict);
            minNIndex = min(minNIndex, neighbourIndex);
            maxNIndex = max(maxNIndex, neighbourIndex);
            newState.array[neighbourIndex].y = state->array[index].y;
        }
        newState.moveDir = dirDown;
        newState.array[index].y += 1;
        break;
    case dirLeft:
        for (i = 0; i < figDict->height[fig.id]; i++) {
            neighbourIndex = findFigure(&newState, fig.x - 1, fig.y + i, figDict);
            minNIndex = min(minNIndex, neighbourIndex);
            maxNIndex = max(maxNIndex, neighbourIndex);
            newState.array[neighbourIndex].x = state->array[index].x + figDict->width[fig.id] - 1;
        }
        newState.array[index].x -= 1;
        newState.moveDir = dirLeft;
        break;
    case dirRight:
        for (i = 0; i < figDict->height[fig.id]; i++) {
            neighbourIndex = findFigure(state, fig.x + figDict->width[fig.id], fig.y + i, figDict);
            minNIndex = min(minNIndex, neighbourIndex);
            maxNIndex = max(maxNIndex, neighbourIndex);
            newState.array[neighbourIndex].x = state->array[index].x;
        }
        newState.array[index].x += 1;
        newState.moveDir = dirRight;
        break;
    case 0:
        debugOut("Error: dir0");
        exit(1);
    }

    minNIndex = min(minNIndex, index);
    maxNIndex = max(maxNIndex, index);
    qsort(&newState.array[minNIndex], maxNIndex - minNIndex + 1, sizeof(Figure), compareFigures);

    return newState;
}

int allSteps(Context *context, const State *currState) {
    int i, x, y, res;
    int maxCnt = context->startPosition.width *context->startPosition.height * 4;
    int emptyCount = 0, possibleMovesCount = 0;

    int *emptyIndexArr = malloc(currState->currLen * sizeof(int));
    int *possibleMoves = malloc(maxCnt * sizeof(int));
    int *possibleMovesDir = malloc(maxCnt * sizeof(int));

    assert(emptyIndexArr != NULL);
    assert(possibleMoves != NULL);
    assert(possibleMovesDir != NULL);

    memset(possibleMovesDir, 0, maxCnt);

    int width = context->startPosition.width, height = context->startPosition.height;

    for (i = 0; i < currState->currLen; i++) {
        if (currState->array[i].id == 0) {
            emptyIndexArr[emptyCount++] = i;
        }
    }

    for (i = 0; i < emptyCount; i++) {
        const int emptyIndex = emptyIndexArr[i];
        x = currState->array[emptyIndex].x;
        y = currState->array[emptyIndex].y;
        if (x + 1 < width) {
			res = findFigure(currState, x + 1, y, &context->figDict);

			if (currState->array[res].id > 0 && canMove(currState, dirLeft, res, &context->figDict)) {
				possibleMoves[possibleMovesCount] = res;
				possibleMovesDir[possibleMovesCount] = dirLeft;
				possibleMovesCount++;
			}
		}
		if (x > 0) {
			res = findFigure(currState, x - 1, y, &context->figDict);

			if (currState->array[res].id > 0 && canMove(currState, dirRight, res, &context->figDict)) {
				possibleMoves[possibleMovesCount] = res;
				possibleMovesDir[possibleMovesCount] = dirRight;
				possibleMovesCount++;
			}
		}
		if (y + 1 < height) {
			res = findFigure(currState, x, y + 1, &context->figDict);

			if (currState->array[res].id > 0 && canMove(currState, dirUp, res, &context->figDict)) {
				possibleMoves[possibleMovesCount] = res;
				possibleMovesDir[possibleMovesCount] = dirUp;
				possibleMovesCount++;
			}
		}
		if (y > 0) {
			res = findFigure(currState, x, y - 1, &context->figDict);

			if (currState->array[res].id > 0 && canMove(currState, dirDown, res, &context->figDict)) {
				possibleMoves[possibleMovesCount] = res;
				possibleMovesDir[possibleMovesCount] = dirDown;
				possibleMovesCount++;
            }
        }
    }    

    for (i = 0; i < possibleMovesCount; i++) {
        context->buffer[i] = move(currState, possibleMovesDir[i], possibleMoves[i], &context->figDict);
    }

    free(emptyIndexArr);
    free(possibleMoves);
    free(possibleMovesDir);

    return possibleMovesCount;
}

Node* addSteps(Context *context) {
    Item *curr = context->queue.first;
    int i, j;

    int count = allSteps(context, curr->data);

    for (i = 0; i < count; i++) {
        // out("\n, count = %d", count);
        // printState(&context->buffer[i], &context->startPosition);
        if (getHistoryItem(&context->history, &context->buffer[i]) == NULL) {
            char hash[128];
            toHashData(&context->buffer[i], hash);
            Node *node = getHistoryItem(&context->history, curr->data);
            addToHistory(&context->history, &context->buffer[i], node);
            Node *newNode = getHistoryItem(&context->history, &context->buffer[i]);
            addToQueue(&context->queue, &context->buffer[i], newNode);
            if (isEndPosition(&context->buffer[i], context->finalPosition)) {
                return newNode;
            }
        }
    }
    return NULL;
}                                                                                   

void getWay(State **way, const Node* node, const Context *context) {
    Node *curr = node;
    int len = 0;
    
    while (curr != NULL) {
        way[len++] = fromHashData(curr, context);
        curr = curr->prev;
    }
}

int getWayLen(const Node *node) {
    Node *curr = node;
    int len = 1;

    while (curr->prev != NULL) {
        curr = curr->prev;
        len++;
    }

    return len;
}

//State* hashTest(const State *state, const Context *context) {
//    int *data;
//    Field f;
//    State *testState;
//    data = malloc(sizeof(int) * state->currLen);
//    toHashData(state, data);
//    initField(&f, context->startPosition.width, context->startPosition.height);
//    initState(&testState, &context->figDict, &f, &context->finalPosition);
//    testState->currLen = state->currLen;
//    testState->node = state->node;
//    testState = fromHashData(data, context);
//    free(data);
//    return testState;
//}

void moveFigOnField(Field *currPos, int x, int y, enum Direction dir) {
    int fWidth = getFigureWidth(currPos, currPos->map[y][x], x, y);
    int fHeight = getFigureHeight(currPos, currPos->map[y][x], x, y);
    int i;

    char letter = currPos->map[y][x];

    switch (dir) {
    case dirUp:
        for (i = 0; i < fWidth; i++) {
            currPos->map[y - 1][x + i] = letter;
            currPos->map[y + fHeight - 1][x + i] = '#';
        }
        
        break;
    case dirDown:
        for (i = 0; i < fWidth; i++) {
            currPos->map[y + fHeight][x + i] = letter;
            currPos->map[y][x + i] = '#';
        }

        break;
    case dirRight:
        for (i = 0; i < fHeight; i++) {
            currPos->map[y + i][x + fWidth] = letter;
            currPos->map[y + i][x] = '#';
        }

        break;
    case dirLeft:
        for (i = 0; i < fHeight; i++) {
            currPos->map[y + i][x - 1] = letter;
            currPos->map[y + i][x + fWidth - 1] = '#';
        }

        break;
    case 0:
        debugOut("Error: dir0");
        exit(1);
    }
}

int search(const Field *startPosition, const FinalPosition *finalPos){
    Context context;
    int result = 0;
    Node *endNode;

    initContext(&context, startPosition, finalPos);

//    addToHistory(&context.history, &context.startState);
//    addToQueue(&context.queue, &context.startState);

    while(!isQueueEmpty(&context.queue)) {
        endNode = addSteps(&context);
        if (endNode == NULL) {
            qPop(&context.queue);
        }
        else {
            Field currField = context.startPosition;
            int wayLen = getWayLen(endNode);
            State **way;
            way = malloc(sizeof(State*) * wayLen);
            assert(way != NULL);
            getWay(way, endNode, &context);
            int i, x, y;
            //out("State #1:");
            printField(&currField);

            for (i = wayLen - 2; i >= 0; i--) {
                //out("State #%i:", wayLen - i);
                if (isDebug) {
                    printState(way[i], &context.startPosition);
                }
                x = way[i + 1]->array[way[i]->figureMoveIndex].x;
                y = way[i + 1]->array[way[i]->figureMoveIndex].y;

                moveFigOnField(&currField, x, y, way[i]->moveDir);
                printField(&currField);
            }
            result = 1;
            break;
        }
    }
    return result;
}
