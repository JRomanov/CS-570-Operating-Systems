/*
James Lee (RedID: 820655947)
James Romanov (RedID: 820821671)
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "structs.h"
#include "pgFunctions.h"
#include "output_mode_helpers.h"
#include "byutr.h"


//Variable Declarations
bool nFlag = false, oFlag = false;
FILE *printFile;
int limit;
int mode;

int levelCount;

bool bitmaskFlag = false;
bool logical2PhysicalFlag = false;
bool page2FrameFlag = false;
bool offsetFlag = false;
bool summaryFlag = false;


//Void parseArguments. This will getopt with the cases and assign certain flags to be true or keep them as false/
void parseArguments(int argc, char **argv) {
    levelCount = argc-2;
    int c;
    while((c = getopt(argc, argv, "n:o")) != -1) { 
        switch(c) {
            case 'n':           //-n flag sets a limit to number of addresses to process
                nFlag = true;
                limit = atoi(optarg);
                levelCount -= 2;
                break;
            case 'o':          

                //Compares the argument to bitmasks, and if so marks the bitmasks flag as true.
                if((strcmp(argv[optind] , "bitmasks") == 0)){
                    bitmaskFlag = true;
                }

                //Compares the argument to logical2physical, and if so marks the logical2physical flag as true.
                if((strcmp(argv[optind] , "logical2physical") == 0)){
                    logical2PhysicalFlag = true;
                }

                //Compares the argument to page2frame, and if so marks the page2frame flag as true.
                if((strcmp(argv[optind] , "page2frame") == 0)){
                    page2FrameFlag = true;
                }

                //Compares the argument to offset, and if so marks the offset flag as true.
                if((strcmp(argv[optind] , "offset") == 0)){
                    offsetFlag = true;
                }

                //Compares the argument to summary, and if so marks the summary flag as true.
                if((strcmp(argv[optind] , "summary") == 0)){
                    summaryFlag = true;
                }
                levelCount -= 2;
                break;
            default:
                summaryFlag = true;
                break;
        }
    }
}

//This method will basically run the code and do all the grunt work for creating the pagetable and such.
void runStart(int argc, char **argv) {
    FILE *traceFile;
    
    /*create the page table and initialize it*/
    PAGETABLE *pageTable = calloc(1, sizeof(PAGETABLE));
    pageTable->levelCount = levelCount;
    int offsetBits = ADDRESS_LENGTH - initializePageTable(pageTable, argv, (argc - levelCount));
    int pageSize = pageByteSize(offsetBits);

    //This is a struct that will take in the tracer file.
    p2AddrTr trace;   
    if ((traceFile = fopen(argv[argc-levelCount-1],"rb")) == NULL) {
        fprintf(stderr,"cannot open %s for reading\n",argv[argc-levelCount-1]);
        exit(1);
    }
    
    //Variable Declarations
    unsigned long addressCount = 0;
    unsigned int address = (unsigned int)trace.addr;
    
    int offsetMask = 0;

    //This for loop will create an offsetMask in order for us to use it in the offset function.
    for(int i =0; i < levelCount; i++){
        offsetMask = offsetMask | pageTable->bitmaskArray[i];
    }
    offsetMask = ~offsetMask;

    //This will keep looping until the end of the file is reached.
    while (!feof(traceFile)) {
        if (nFlag)
            if (addressCount >= limit)      //break out of loop if limit is reached
                break;

        if (NextAddress(traceFile, &trace)) {
            unsigned int address = (unsigned int)trace.addr;
            uint32_t pages[levelCount];

            pageInsert(pageTable, address, pageTable->frameCount);

            //This will create a pages array where it will store the logical to page for pagemap.
            for(int j = 0; j < levelCount; j++){
                pages[j] = logicalToPage(address, pageTable->bitmaskArray[j], pageTable->shiftArray[j]);
            }

            //If the offsetFlag is true, then we will run the logical2offset function.
            if(offsetFlag){
                report_logical2offset(address, address & offsetMask);
            }

            //If the page2FrameFlag is true then it will do a pageLookup then run the report pagemap.
            if (page2FrameFlag) {
                MAP *map = pageLookup(pageTable, address);
                report_pagemap(address,levelCount, pages, map->frame);
            } 

            //If the logical2PhysicalFlag is true then it will pagelookup then run the logical2physical function.
            if (logical2PhysicalFlag) {
                MAP *map = pageLookup(pageTable, address);
                unsigned int translated = address;
                translated &= ((1 << offsetBits) - 1);
                translated += (map->frame << (offsetBits));
                report_logical2physical(address, translated);
            }

            
           

        }
        //Increments addressCount.
        addressCount++;
    }
    
    int byteAmount = addressCount;

    //If bitmaskFlag is true then we will run the report bitmasks function.
    if (bitmaskFlag){
        report_bitmasks(levelCount, pageTable->bitmaskArray);
    }

    //If the summaryFlag is true then we will run the report summary function.
    if(summaryFlag){
        report_summary(pageSize, pageTable->hits, addressCount, pageTable->frameCount, pageTable->bytesUsed);
    }

}

//Our main function will just call the two previous functions and return 0.
int main(int argc, char **argv) {
    parseArguments(argc, argv);

    runStart(argc, argv);

    return 0;
}