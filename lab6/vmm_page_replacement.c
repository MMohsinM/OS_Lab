//
// Created by mohsin on 30/5/21.
//
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define TLB_SIZE 16
#define PAGES 256
#define FRAMES 128
#define PAGE_MASK 255

#define PAGE_SIZE 256
#define OFFSET_BITS 8
#define OFFSET_MASK 255

#define MEMORY_SIZE FRAMES * PAGE_SIZE

// Max number of characters per line of input file to read.
#define BUFFER_SIZE 10

// TLB ASSOCIATED GLOBAL DEFINITIONS
// TLB Cache for fast look up
struct tlbentry {
    unsigned char logical;
    unsigned char physical;
};
// TLB is kept track of as a circular array, with the oldest element being overwritten once the TLB is full.
struct tlbentry tlb[TLB_SIZE];
int tlbindex = 0; // to frame TLB array


// PAGE TABLE ASSOCIATED GLOBAL DEFINITIONS
typedef struct {
    int frame;
    unsigned int valid:1;
}pagetable_entry;
// pagetable[logical_page] is the physical page number for logical page. Value is -1 if that logical page isn't yet in the table.
pagetable_entry pagetable[PAGES];


// PHYSICAL MEMORY ASSOCIATED GLOBAL DEFINITIONS
signed char physical_memory[MEMORY_SIZE];

// PAGE REPLACEMENT ASSOCIATED DEFINITIONS
//FIFO for Page Replacement Index
int fifo_page_replacer_index=0;
// This array holds the page numbers and updates the record with FIFO principle
int fifo_page_replacer_array[FRAMES]={0};
int get_victim_page(){
    int victim_page;
    return victim_page = fifo_page_replacer_array[fifo_page_replacer_index];
}

void add_to_fifo_page_replacer_array(unsigned char logical) {
    int* entry = &fifo_page_replacer_array[fifo_page_replacer_index];
    fifo_page_replacer_index++;
    if (fifo_page_replacer_index == FRAMES)
    { fifo_page_replacer_index = 0;}
    *entry = logical;
}


// Pointer to memory mapped backing file
signed char *backing;


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



void page_fault_handler(List free_frame_list, int logical_page, int *page_faults, int *physical_frame) {
    int victim_page;
    (*page_faults)++;

    (*physical_frame) = Pop(free_frame_list);

    if (*physical_frame == -1){
        // Page Replacement using FIFO
        victim_page = get_victim_page();
        //printf("FIFO INDEX = %d\n", fifo_page_replacer_index);
        //printf("victim page is %d ", victim_page);
        pagetable[victim_page].valid = 0;
        *physical_frame = pagetable[victim_page].frame;
        //printf("victim frame is %d\n", *physical_frame);
    }

    // Copy page from backing file into physical memory
    memcpy(physical_memory + (*physical_frame) * PAGE_SIZE, backing + logical_page * PAGE_SIZE, PAGE_SIZE);

    // Also add page to page table
    pagetable[logical_page].frame = (*physical_frame);
    pagetable[logical_page].valid = 1;
}



//
int main(int argc, const char *argv[])
{
    // Free Frame List Initialization
    List free_frame_list = NULL;
    free_frame_list = MakeEmptyList(free_frame_list);

    const char *backing_file = argv[1];
    int backing_fd = open(backing_file, O_RDONLY);
    if (backing_fd < 0)
        fprintf(stderr,"Cannot Open File %s", argv[1]);

    backing = mmap(0, PAGE_SIZE*PAGES, PROT_READ, MAP_PRIVATE, backing_fd, 0);

    const char *input_file = argv[2];
    FILE *input_fp = fopen(input_file, "r");
    if (input_fp < 0)
        fprintf(stderr,"Cannot Open File %s", argv[2]);

    // Fill page table entries with -1 for initially empty table.
    int i;
    for (i = 0; i < PAGES; i++) {
        pagetable[i].valid = 0;
    }

    //Add All frames to the free frame list
    for (i=0; i<FRAMES; i++){
        Push(i, free_frame_list);
    }

    // Character buffer for reading lines of input file.
    char buffer[BUFFER_SIZE];

    // Data we need to keep track of to compute stats at end.
    int total_addresses = 0;
    int tlb_hits = 0;
    int page_faults = 0;

    // Number of the next unallocated physical page in main memory


    while (fgets(buffer, BUFFER_SIZE, input_fp) != NULL) {
        total_addresses++;
        int logical_address = atoi(buffer);

        // Extracting page number and page offset by bit shifting and masking
        int offset = logical_address & OFFSET_MASK;
        int logical_page = (logical_address >> OFFSET_BITS) & PAGE_MASK;

        // page search...
        int physical_frame = search_tlb(logical_page);
        // TLB hit
        if (physical_frame != -1)
            tlb_hits++;
        //TLB MISS
        else {
            // Page fault
            if (!pagetable[logical_page].valid) {
                page_fault_handler(free_frame_list, logical_page, &page_faults, &physical_frame);
                add_to_fifo_page_replacer_array(logical_page);
            } else{
                physical_frame = pagetable[logical_page].frame;
            }

            // Also add (page, frame) to TLB
            add_to_tlb(logical_page, physical_frame);
        }


        // physical address
        int physical_address = (physical_frame << OFFSET_BITS) | offset;

        // Byte value at the physical address in physical memory
        signed char value = physical_memory[physical_frame * PAGE_SIZE + offset];


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

