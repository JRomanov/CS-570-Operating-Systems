/*
James Lee (RedID: 820655947)
James Romanov (RedID: 820821671)
*/

#include <stdio.h>
#include <stdlib.h>
#include "pgFunctions.h"

//This function will initialize the Pagetable and allocate memory for each variable. 
//Calloc was used to initialize to 0, even though this will be a bit slower than malloc.
int initializePageTable(PAGETABLE *pageTable, char **argv, int position) {
    pageTable->shiftArray = calloc(pageTable->levelCount, sizeof(int));
    pageTable->bitmaskArray = calloc(pageTable->levelCount, sizeof(unsigned int));
    pageTable->entryCount = calloc(pageTable->levelCount, sizeof(unsigned int));

    int bytesUsed = 0;
    int bitsUsed = 0;
    int i;
    for (i = 0; i < pageTable->levelCount; i++) {
        int levelBits = atoi(argv[position]);
        pageTable->shiftArray[i] = ADDRESS_LENGTH - bitsUsed - levelBits;

        //This will calculate the bitmask.
        int maskStart = ADDRESS_LENGTH - bitsUsed;
        pageTable->bitmaskArray[i] = calcBitmask(maskStart, levelBits);
        pageTable->entryCount[i] = (1 << levelBits);
        bitsUsed += levelBits;
        position++;

        //This will check for an error if the user inputted more than 32 bits.
        if (bitsUsed >= ADDRESS_LENGTH) {
            fprintf(stderr,"The Adress Length must be maximum 32 bits.\n");
            exit(1);
        }
    }
    
    //Initilizing to 0.
    pageTable->hits = 0;
    pageTable->misses = 0;
    pageTable->rootNode = createLevel(pageTable, pageTable->rootNode, 0);
    return bitsUsed;
}

//This will calculate the bitmask and return the mask.
unsigned int calcBitmask(int start, int length) {
    unsigned int mask = (1 << length) - 1; 
    mask <<= (start - length);
    return mask;
}

// This will return the index of the specific page using the address, bitmask, and shift.
unsigned int logicalToPage(unsigned int logicalAddress, unsigned int bitmask, unsigned int shift) {
    unsigned int page = logicalAddress;
    page &= bitmask;
    page >>= shift;
    return page;
}

//This function will just call the helper function. They couldn't be the same name.
void pageInsert(PAGETABLE *pageTable, unsigned int logicalAddress, unsigned int frame) {
    pageInsertHelper(pageTable->rootNode, logicalAddress, frame);
}

//This function will create new levels if need be, and will also insert a new address into the page table.
void pageInsertHelper(LEVEL *level, unsigned int logicalAddress, unsigned int frame) {
    unsigned int index = logicalToPage(logicalAddress, level->pageTable->bitmaskArray[level->depth], level->pageTable->shiftArray[level->depth]);

    //If the level is a leafNode then check if it is not valid or not, then instantiate fram and frameCount.
    if (level->isLeafNode) { 
        if(!level->map[index].isValid){
            level->map[index].frame = frame;
            level->pageTable->frameCount++;
        }else{
            level->pageTable->hits++;
        }
        level->map[index].isValid = true;
        
    }
    else {    
        if (level->nextLevel[index] == NULL)
            level->nextLevel[index] = createLevel(level->pageTable, level, level->depth + 1);
        pageInsertHelper(level->nextLevel[index], logicalAddress, frame);
    }
}

//This function will call the helper function. They couldn't be the same name.
MAP * pageLookup(PAGETABLE *pageTable, unsigned int logicalAddress) {
    return pageLookupHelper(pageTable->rootNode, logicalAddress);
}

//This function will will check if the address works, and if not it will return Null.
MAP * pageLookupHelper(LEVEL *level, unsigned int logicalAddress) {
    unsigned int index = logicalToPage(logicalAddress, level->pageTable->bitmaskArray[level->depth], level->pageTable->shiftArray[level->depth]);
    
    
    //Checks if it is the leaf note, and will return index.
    if (level->isLeafNode) {
        if (level->map[index].isValid){
            return &level->map[index];
        }
        else{
            return NULL;
        }
    }
    else {
        if (level->nextLevel[index] == NULL){ 
            return NULL;
        }
        return pageLookupHelper(level->nextLevel[index], logicalAddress);
    }
}

//This will create a new level and allocate memory for the pagetable.
LEVEL * createLevel(PAGETABLE *pageTable, LEVEL *level, int depth) {

    //Initializing the level attributes.
    level = calloc(1, sizeof(LEVEL));
    level->pageTable = pageTable;
    level->depth = depth;
    level->isLeafNode = (depth+1 >= pageTable->levelCount);

    //Checks to see if it is a leafnode.
    if (level->isLeafNode) {   
        level->map = calloc(pageTable->entryCount[depth], sizeof(MAP));
        pageTable->bytesUsed = pageTable->entryCount[depth] * sizeof(MAP);
        for (int i = 0; i < pageTable->entryCount[depth]; i++) {
            level->map[i].isValid = false;
        }
    }
    else {   
        //This will allocate memory.
        level->nextLevel = calloc(pageTable->entryCount[depth], sizeof(LEVEL *));
        pageTable->bytesUsed = pageTable->entryCount[depth] * sizeof(LEVEL *);
    }
    return level;
}

//This function will calculate the Pagesize for one of the main fucntions.
int pageByteSize(unsigned int bitOffset){
    int pageSize = 0;

    pageSize += (1 << bitOffset);

    return pageSize;
}