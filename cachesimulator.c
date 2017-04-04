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
#define CacheAssociativityExp 3
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
  uint32_t tag;
} BlockStruct;

int cacheHit = 0;
int cacheMiss = 0;

BlockStruct cache[LineSize][CacheAssociativity] = {};

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

void replacement(uint32_t address) {
  int BlockColumn = 0;
  for(;BlockColumn < CacheAssociativity; ++BlockColumn) {
    //If its not valid
    if(!cache[(address >> BlockSizeExp) & LineSizeMask][BlockColumn].valid) {
      //Set the tag to the address

      cache[(address >> BlockSizeExp) & LineSizeMask][BlockColumn].tag = (address >> (BlockSizeExp + LineSizeExp)) & TagSizeMask;
      //Set it to valid
      cache[(address >> BlockSizeExp) & LineSizeMask][BlockColumn].valid = true;
    }
  }
}
//Don't ask what is going on in here
void ReadFromTraceFile() {
  //Open the file
  inFile = fopen("AddressTrace_LastIndex.bin", "r");
  if (inFile != NULL) {
    printf("Read file correctly\n");
  }
  uint32_t address;
  //Read in each address from the file 32-bits each
  while (fread(&address, (AddressSizeExp >> 3), 1, inFile) != 0) {
    uint32_t Line = (address >> BlockSizeExp) & LineSizeMask;
    uint32_t Tag = ((address >> (BlockSizeExp + LineSizeExp)) & TagSizeMask);
    //Used for associativity
    int BlockColumn = 0;
    bool Hit = false;
    //Loop over the columns
    for(;BlockColumn < CacheAssociativity; ++BlockColumn) {
      //if the cache[Line][Column]'s tag is equal to read in tag
      if(cache[Line][BlockColumn].tag == Tag) {
        //We get a hit and move on
        ++cacheHit;
        //Set block as valid
        cache[Line][BlockColumn].valid = true;
        Hit = true;
        //break out of for loop
        break;
      }
      else {
        //Set the block to invalid
        cache[Line][BlockColumn].valid = false;

      }
      //printf("%d\n", TagSizeMask);

    }
    if(Hit == false) {
      //printf("Line: %d\n", Line);
      //printf("Tag: %d\n", Tag);
      //Otherwise we got a miss
      ++cacheMiss;
      //Call Replacement method
      replacement(address);
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
