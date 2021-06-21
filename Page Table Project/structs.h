/*
James Lee (RedID: 820655947)
James Romanov (RedID: 820821671)
*/

#ifndef _pagetable_h
#define _pagetable_h

typedef enum {false, true} bool;

typedef struct PAGETABLE {
    struct LEVEL *rootNode;
    int levelCount;
    int frameCount;
    int hits;
    int misses;
    int bytesUsed;
    unsigned int *bitmaskArray;
    int *shiftArray;
    int *entryCount;
} PAGETABLE;

typedef struct MAP {     
    bool isValid;
    unsigned int frame;
} MAP;

typedef struct LEVEL {   
    bool isLeafNode;
    struct PAGETABLE *pageTable;
    struct LEVEL **nextLevel;
    struct MAP *map;
    int depth;
} LEVEL;

#endif