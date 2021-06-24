
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define TLB_SIZE 16
#define PAGES 256
#define PAGE_MASK 255

#define PAGE_SIZE 256
#define OFFSET_BITS 8
#define OFFSET_MASK 255

#define MEMORY_SIZE PAGES * PAGE_SIZE

// Max number of characters per line of input file to read.
#define BUFFER_SIZE 10

// TLB Cache for fast look up
struct tlbentry {
    unsigned char logical[TLB_SIZE];
    unsigned char physical[TLB_SIZE];
    int entryAge[TLB_SIZE];
} ;

int getOldestEntry(int tlbSize);


typedef enum { false = 0, true = !false } bool; // Simple true or false boolean


// TLB is kept track of as a circular array, with the oldest element being overwritten once the TLB is full.
struct tlbentry tlb[TLB_SIZE];

int tlbindex = 0; // to index TLB array

// pagetable[logical_page] is the physical page number for logical page. Value is -1 if that logical page isn't yet in the table.
int pagetable[PAGES];

signed char physical_memory[MEMORY_SIZE];

// Pointer to memory mapped backing file
signed char *backing;



// Helping functions

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

/* Returns the physical address from TLB or -1 if not present. */
int search_tlb(unsigned char logical_page) {
    int i;
    for (i = 0; i < TLB_SIZE; i++) {   // Linear Search
        struct tlbentry *entry = &tlb[i];
        
        if (strcmp(&(entry->logical[i]), &logical_page)==0) {
            return entry->physical[i];
        }
    }
    
    return -1;
}




void tlbLRUinsert(unsigned char logical, unsigned char physical)
{
    struct tlbentry *entry = &tlb[tlbindex];
    tlbindex++;
    bool freeSpotFound = false;
    bool alreadyThere = false;
    int replaceIndex = -1;
    if (tlbindex == TLB_SIZE)
    	{ tlbindex = 0;}
    

    //Find the index to replace and increment age for all other entries(age in the TLB)
    for (int i = 0; i < TLB_SIZE; i++) {
        if ((strcmp(&(entry->logical[i]), &logical))!=0 && (entry->logical[i] != 0)) {
            // If entry is not in TLB and not a free spot, increment its age
            entry->entryAge[i]++;
        }
        else if ((strcmp(&(entry->logical[i]), &logical))!=0 && (entry->logical[i] == 0)) { //second argument here refers to whether the tlb space is free
            // Available spot in TLB found
            if (!freeSpotFound) {
                replaceIndex = i;
                freeSpotFound = true; //this flag is updated as a result of the free location
            }
        }
        else if ((strcmp(&(entry->logical[i]),&logical))==0) {
            // Reset entrys' age; in case of duplicate entries which want to be inserted
            if(!alreadyThere) {
                entry->entryAge[i]= 0;
                alreadyThere = true;  //flagupdated for already existing entry
            }
        }
    }

    //if-else construct for insertion of addresses
    if (alreadyThere) {
        return;  //no need of insertion for a duplicate entry
    }
    else if (freeSpotFound) { // If we found a free spot, insert
        entry->logical[replaceIndex]= logical;    // Insert into free spot
        entry->physical[replaceIndex] = physical;
        entry->entryAge[replaceIndex] = 0;
    }
    else { // No more free spots 
        replaceIndex = getOldestEntry(TLB_SIZE);
        entry->logical[replaceIndex] = logical;    // Insert into oldest entry i.e the least used one. Both logical and physical addresses are being inserted
        entry->physical[replaceIndex] = physical;  
        entry->entryAge[replaceIndex] = 0;

    } 
    
    
} 
int getOldestEntry(int tlbSize) {
  int i, max, index;
  struct tlbentry *entry = &tlb[tlbindex];

  max = entry->entryAge[0];
  index = 0;

  for (i = 1; i < tlbSize; i++) {
    if (entry->entryAge[i] > max) {
       index = i;
       max = entry->entryAge[i];  //retrieves max age of element inside the TLB
    }
  }
  return index; //retrieves the index with oldest age in TLB
  
 } 

//
int main(int argc, const char *argv[])
{   
    const char *backing_file = argv[1]; 
    int backing_fd = open(backing_file, O_RDONLY);
    backing = mmap(0, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, backing_fd, 0); 
    
    const char *input_file = argv[2];
    FILE *input_fp = fopen(input_file, "r");
    
    // Fill page table entries with -1 for initially empty table.
    int i;
    for (i = 0; i < PAGES; i++) {
        pagetable[i] = -1;
    }
    
    // Character buffer for reading lines of input file.
    char buffer[BUFFER_SIZE];
    
    // Data we need to keep track of to compute stats at end.
    int total_addresses = 0;
    int tlb_hits = 0;
    int page_faults = 0;
    
    // Number of the next unallocated physical page in main memory
    unsigned char free_page = 0;
    
    while (fgets(buffer, BUFFER_SIZE, input_fp) != NULL) {
        total_addresses++;
        int logical_address = atoi(buffer);
        
      // Extracting page number and page offset by bit shifting and masking
        int offset = logical_address & OFFSET_MASK;
        int logical_page = (logical_address >> OFFSET_BITS) & PAGE_MASK;
        
        // page search...
        int physical_page = search_tlb(logical_page);
        // TLB hit
        if (physical_page != -1) {
            tlb_hits++;
            
        }
        // TLB miss
         else {
            physical_page = pagetable[logical_page];
            
            // Page fault
            if (physical_page == -1) {
                page_faults++;
                
                physical_page = free_page;
                free_page++;
                
                // Copy page from backing file into physical memory
                memcpy(physical_memory + physical_page * PAGE_SIZE, backing + logical_page * PAGE_SIZE, PAGE_SIZE);
                
                // Also add page to page table
                pagetable[logical_page] = physical_page;
            }
            
            // Also add (page, frame) to TLB
           
            tlbLRUinsert(logical_page, physical_page);
            
        }
        
        // physical address 
        int physical_address = (physical_page << OFFSET_BITS) | offset;
        
        // Byte value at the physical address in physical memory
        signed char value = physical_memory[physical_page * PAGE_SIZE + offset];
        
        
        // results...
        printf("Virtual address: %d Physical address: %d Value: %d\n", logical_address, physical_address, value);
    }
    
    
    // Stats of VMM
    printf("Number of Translated Addresses = %d\n", total_addresses);
    printf("Page Faults = %d\n", page_faults);
    printf("Page Fault Rate = %.3f\n", page_faults / (1. * total_addresses));
    printf("TLB Hits = %d\n", tlb_hits);
    printf("TLB Hit Rate = %.3f\n", tlb_hits / (1. * total_addresses));
    
    return 0;
}


