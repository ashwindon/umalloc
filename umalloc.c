#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "umalloc.h"


char* next(char* curr) {
   //Navigation mechanism to go to next blocks.
   char* follows = curr + ((Metadata*)curr)->blockSize + sizeof(Metadata);
   if(follows >= &MEM[MEM_SIZE]) {
      return NULL;
   }
   return follows;
}
void combine() {
   //Combine multiple free blocks as one.
   char* currNode = &MEM[0];
   char* nextToCurr = NULL;
   //Travese from start and check for free spaces and combine them in one pass.

   size_t totalFreeSize = 0;
   while(currNode!=NULL && currNode < &MEM[MEM_SIZE]) {
      if(((Metadata*)currNode)->used == 'f') {
         //nextToCurr -> pointer to next memory block
         nextToCurr = next(currNode);
         if(nextToCurr == NULL || nextToCurr >= &MEM[MEM_SIZE]) {
            return;
         }

         totalFreeSize += ((Metadata*)nextToCurr)->blockSize + sizeof(Metadata);
         if(((Metadata*)nextToCurr)->used == 'f') {
            //If next block is not occupied add cumulative block size to current pointer and change next pointer.
            ((Metadata*)currNode)->blockSize += totalFreeSize;
            totalFreeSize = 0;
            nextToCurr = next(nextToCurr);
         }
         else{
            //If next block is occupied the just care to move current pointer as no cumulative addition is required.
            currNode = next(currNode);
         }
      }
      else {
         //If current block is occupied just move current node to next block.
         currNode = next(currNode);
      }
   }
}
void* allocate(char* curr, size_t size) {
   size_t reqSize = sizeof(Metadata) + size;
   size_t availableSize = ((Metadata*)curr)->blockSize;

   size_t remainder = availableSize - reqSize;

   //update available block size as occupied and set block size equal to requested block size.

   if(availableSize-size-1 >= sizeof(Metadata)){
      //If it is possible to create new allocate block for future allocation. i.e it fragment that would be created after allocation is big enough then run this code block.
      char* newMetadata = curr + reqSize;
      ((Metadata *)curr)->used = 't';
       ((Metadata *)curr)->blockSize = size;

      ((Metadata *)newMetadata)->used = 'f';
      ((Metadata *)newMetadata)->blockSize = remainder;
   }
   else {
      //If future data and metadata doesn't fit in a fragment that will be created by current allocation allocate whole of available size to current allocation.
      ((Metadata *)curr)->used = 't';
      ((Metadata *)curr)->blockSize = availableSize;
      
   }

   return (void*)(curr + sizeof(Metadata));
}


void* umalloc(size_t size,char *file, int line) {

   //check if requested size lies within MEM allocator.
   //Since we also have to allocate memory for metadata take care of that as well.
   if(size <= 0)
   {
      printf("%s : %d Error: Size requested is either 0 or negative\n", file, line);
      return NULL;
   }
   if(size > MEM_SIZE - sizeof(Metadata)) {
      printf("%s : %d Error: Memory overflow!!\n", file, line);
      return NULL;
   }

   if(memoryInit == 0) {

      ((Metadata*)&MEM[0])->used = 'f'; //f -> false t -> true.
      ((Metadata*)&MEM[0])->blockSize = MEM_SIZE - sizeof(Metadata);

      memoryInit = 1;
   }
   // Traverse MEM to find first fit greater than or equal to requested size

   //actualSize -> size + metadata
   int fragMem = 0;
   char* itr = &MEM[0];
   size_t actualSize = size;
   size_t blockSizeAvaliable = 0;
   while (itr < &MEM[MEM_SIZE] && itr!= NULL)
   {
      // printf("Block Size - %d  FLAG - %c\n", ((Metadata*)itr)->blockSize, ((Metadata*)itr)->used);

      if(((Metadata*)itr) -> used == 'f' && ((Metadata*)itr)->blockSize >= actualSize) {
         //got the starting adress where data needs to be stored
         if(((Metadata*)itr) -> used == 'f' && ((Metadata*)itr)->blockSize == size) {
               ((Metadata*)itr)->used = 't';
               return (void*)itr + sizeof(Metadata);
         }
         break;
      }
      else if(((Metadata*)itr) -> used == 'f' && ((Metadata*)itr)->blockSize < actualSize) {
         //accumalate empty fragments.
         fragMem=fragMem+((Metadata*)itr)->blockSize;
      }

      itr = next(itr);
   }

   if(itr == NULL) 
   {
      printf("Fragment size = %d\n", fragMem);
      if(size<= fragMem) {
         //fragements add upto or exceed requested size but still cannot allocate because memory block is not contiguous and cant be server at once.
         printf("%s : %d Error: Enough Memory but there is no large contiguous enough memory block for allocation\n", file, line);
         return NULL;
      }
      else if(fragMem == 0){
         //Latest requested block cannot be added.
         printf("%s : %d Error: No free Memory available\n", file, line);
         return NULL;
      }
      //Available 3MB requested 4MB case - though memory is not full still requires more Memory
      printf("%s : %d Error: Memory is not full but there is not enough to accomadate request\n", file, line);
      return NULL;
   }
   return allocate(itr, size);


}

void ufree(void* addressToBeFreed, char *file, int line) {
   char *curr = &MEM[0];
   char* metadataToBeFreed = (char*)((char*)addressToBeFreed - sizeof(Metadata));
 
   if(addressToBeFreed == NULL) {
      printf("%s : %d Error: Can't free for NULL\n", file, line);
      return;
   }

   //Check If address to be freed lies with MEM Allocator area 
   //if addressToBeFreed is less then make sure requested address doesnot lie within metadata.
   if((char*)addressToBeFreed < &MEM[0] + sizeof(Metadata) || (char*)addressToBeFreed > &MEM[MEM_SIZE]) {
      printf("%s : %d Error: Can't be freed Memory : Out of bounds\n", file, line);
      return;
   }

   if (((Metadata*)((char*)addressToBeFreed - sizeof(Metadata))) -> used == '\0')
   {
      printf("%s : %d Error: You are trying to free from inaccesible memory address\n", file, line);
      return;
   }

   while(curr < &MEM[MEM_SIZE] && curr != NULL) {
         // printf("Current Meatadata Used ?? ======== %c, %d\n", ((Metadata *)curr)->used, curr);
      if(curr == metadataToBeFreed) {
         if(((Metadata *)curr)->used == 'f') {
            printf("%s : %d Error: Requested address block is already freed and doesn't contain data\n",file, line);
            //printf("Requested address block is already freed and doesn't contain data\n");
            return;
         }
         ((Metadata *)curr)->used = 'f';
         combine();
         return;
      }
      
      curr = next(curr);
   }
   return;
}
//print metadata
void prettyPrint()
{
    printf("=================Printing Metadata==========================\n");
   
    char *iterator = &MEM[0];
    int cnt=0;
    long tru =0;
    while(cnt < 5)
    {
        printf("Metadata Address: %p\n", iterator);
        printf("Block Size: %d\n", ((Metadata*)iterator)->blockSize);
        printf("Block Allocated flag: %c\n", ((Metadata*)iterator)->used);
        iterator = iterator + sizeof(Metadata) + ((Metadata*)iterator)->blockSize;
        cnt++;
    }
    printf("=============================END================================\n");
}