
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
    unsigned char logical;
    unsigned char physical;
};

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
        
        if (entry->logical == logical_page) {
            return entry->physical;
        }
    }
    
    return -1;
}

/* Adds the specified mapping to the TLB, replacing the oldest mapping (FIFO replacement). */
void add_to_tlb(unsigned char logical, unsigned char physical) {
    struct tlbentry *entry = &tlb[tlbindex];
    tlbindex++;
    if (tlbindex == TLB_SIZE)
    	{ tlbindex = 0;}
    entry->logical = logical;
    entry->physical = physical;
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
            add_to_tlb(logical_page, physical_page);
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


