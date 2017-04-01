#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

//Size of the cache
#define CacheSizeExp 15
#define CacheSize (1 << CacheSizeExp)

//Address Size
#define AddressSizeExp 32

//Cache Size
#define CacheAssociativityExp 0
#define CacheAssociativity (1 << CacheAssociativityExp)

//Block Size
#define BlockSizeExp 6
#define BlockSize (1 << BlockSizeExp)
#define BlockSizeMask (BlockSize - 1)

//Line Size
#define LineSizeExp (CacheSizeExp - (CacheAssociativityExp + BlockSizeExp))
#define LineSize (1 << LineSizeExp)
#define LineSizeMask (LineSize - 1)

//Tag Size
#define TagExp (AddressSizeExp - (BlockSizeExp + LineSizeExp))
#define TagSize (1 << TagExp)
#define TagSizeMask (TagSize - 1)

FILE* inFile;

typedef struct BlockStruct {
  bool valid;
  time_t time;
  char* tag;
} BlockStruct;

int cacheHit = 0;
int cacheMiss = 0;


void PrintParameters() {
  // Print cache parameters
  printf("Cache Size Exp: %d\n", CacheSizeExp);
  printf("Cache Size: %d\n", CacheSize);
  printf("Address Size Exp: %d\n", AddressSizeExp);
  printf("Block Size Exp: %d\n", BlockSizeExp);
  printf("Block Size: %d\n", BlockSize);
  printf("Block Size Mask: %d\n", BlockSizeMask);
  printf("Line Size Exp: %d\n", LineSizeExp);
  printf("Line Size: %d\n", LineSize);
  printf("Line Size Mask: %d\n", LineSizeMask);
  printf("Tag Exp: %d\n", TagExp);
  printf("Tag Size: %d\n", TagSize);
  printf("Tag Size Mask: %d\n", TagSizeMask);
}

//Don't ask what is going on in here
void ReadFromTraceFile() {

  inFile = fopen("AddressTrace_FirstIndex.bin", "r");
  if (inFile != NULL) {
    printf("Read file correctly\n");
  }

  //BlockStruct** cache = (BlockStruct**)malloc(LineSize * CacheAssociativity * sizeof(BlockStruct));
  BlockStruct cache[LineSize][CacheAssociativity];
  uint32_t address;
  while (fread(&address, (AddressSizeExp >> 3), 1, inFile) != 0) {
    //If the tag at the line is equal to the new tag
    // if(cache[(address >> BlockSizeExp) & LineSizeMask].tag == ((address >> (BlockSizeExp + LineSizeExp)) & TagSizeMask)) {
    int BlockColumn = 0;
    for(;BlockColumn < CacheAssociativity; ++BlockColumn) {
      // printf("Cache row: %d\n", (address >> BlockSizeExp) & LineSizeMask);
      // printf("Cache Column %d\n", BlockColumn);
      // printf("Cache hit: %p\n", cache[(address >> BlockSizeExp) & LineSizeMask][BlockColumn].tag);
      if(cache[(address >> BlockSizeExp) & LineSizeMask][BlockColumn].tag == ((address >> (BlockSizeExp + LineSizeExp)) & TagSizeMask)) {
        //We get a hit and move on
        ++cacheHit;
      }
      else
      {  //We get a miss
        ++cacheMiss;
        cache[(address >> BlockSizeExp) & LineSizeMask][BlockColumn].tag = (address >> (BlockSizeExp + LineSizeExp)) & TagSizeMask;
        //TODO:Fifo replacement
      }
    }

  }
  printf("Cache hit: %d\n", cacheHit);
  printf("Cache miss: %d\n", cacheMiss);
}
void main(void) {
  //Print what the compiler stuff
  PrintParameters();
  //Read in the trace file
  ReadFromTraceFile();
}
