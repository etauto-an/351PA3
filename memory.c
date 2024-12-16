#include "memory.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * Initializes the memory system.
 *
 * Args:
 *   memory (Memory*): Pointer to the `Memory` structure to initialize.
 *   total_memory (int): Total size of the memory in KB.
 *   page_size (int): Size of each page in KB.
 *
 * Behavior:
 *   - Divides the total memory into pages of the specified size.
 *   - Creates and initializes the page table to track memory allocation (-1
 *     indicates free pages).
 */
void init_memory(Memory *memory, int total_memory, int page_size) {
    memory->total_memory = total_memory;
    memory->page_size = page_size;
    memory->total_pages = total_memory / page_size;
    memory->page_table = malloc(memory->total_pages * sizeof(int));

    // Initialize all pages as free (-1 means free)
    for (int i = 0; i < memory->total_pages; i++) {
        memory->page_table[i] = -1;
    }
}

/**
 * Allocates memory for a process in the memory system.
 *
 * Args:
 *   memory (Memory*): Pointer to the `Memory` structure.
 *   process_id (int): ID of the process requesting memory.
 *   num_pieces (int): Number of memory segments required by the process.
 *   piece_sizes (int*): Array containing the sizes of each memory segment (in KB).
 *
 * Returns:
 *   int: 1 if memory allocation is successful, 0 otherwise.
 *
 * Behavior:
 *   - Checks if sufficient free pages are available to allocate the required memory.
 *   - Allocates memory in chunks corresponding to the size of each piece.
 *   - If memory is insufficient at any stage, all previously allocated pages for this
 *     process are freed, and the function returns 0.
 */
int allocate_memory(Memory *memory, int process_id, int num_pieces, int *piece_sizes) {
    int total_pages_needed = 0;
    for (int i = 0; i < num_pieces; i++) {
        int pages_for_piece = (piece_sizes[i] + memory->page_size - 1) / memory->page_size;
        total_pages_needed += pages_for_piece;
    }

    // Count free pages
    int free_pages = 0;
    for (int i = 0; i < memory->total_pages; i++) {
        if (memory->page_table[i] == -1) {
            free_pages++;
        }
    }

    // If not enough pages, fail immediately
    if (free_pages < total_pages_needed) {
        return 0; // Not enough memory, must wait
    }

    // Enough memory is available, proceed to allocate
    for (int i = 0; i < num_pieces; i++) {
        int pages_needed = (piece_sizes[i] + memory->page_size - 1) / memory->page_size;
        int pages_allocated = 0;

        for (int j = 0; j < memory->total_pages && pages_allocated < pages_needed; j++) {
            if (memory->page_table[j] == -1) {
                memory->page_table[j] = process_id;
                pages_allocated++;
            }
        }

        // Should always succeed since we checked beforehand. If not:
        if (pages_allocated < pages_needed) {
            // Rollback any allocated pages
            for (int k = 0; k < memory->total_pages; k++) {
                if (memory->page_table[k] == process_id) {
                    memory->page_table[k] = -1;
                }
            }
            return 0; // Fail allocation
        }
    }

    return 1; // Allocation successful
}

/**
 * Deallocates memory for a process in the memory system.
 *
 * Args:
 *   memory (Memory*): Pointer to the `Memory` structure.
 *   process_id (int): ID of the process whose memory is to be deallocated.
 *
 * Behavior:
 *   - Frees all pages in memory that are currently allocated to the specified process.
 */
void deallocate_memory(Memory *memory, int process_id) {
    for (int i = 0; i < memory->total_pages; i++) {
        if (memory->page_table[i] == process_id) {
            memory->page_table[i] = -1;  // Mark page as free
        }
    }
}

void print_memory_map(Memory *memory, int page_size) {
    printf("       Memory Map:\n");

    int start = -1;  // Start of a free range
    int page_number[1000] = {0};  // Track page numbers for processes up to ID 1000

    for (int i = 0; i < memory->total_pages; i++) {
        int start_address = i * page_size;
        int end_address = (i + 1) * page_size - 1;

        if (memory->page_table[i] == -1) {  // Free frame
            if (start == -1) start = start_address;  // Mark the start of free range
        } else {
            // If we were tracking a free range, print it
            if (start != -1) {
                printf("                  %d-%d: Free frame(s)\n", start, start_address - 1);
                start = -1;
            }

            // Print allocated frames
            int process_id = memory->page_table[i];
            page_number[process_id]++;  // Increment the page count for the process

            printf("                  %d-%d: Process %d, Page %d\n",
                   start_address, end_address, process_id, page_number[process_id]);
        }
    }

    // Print any remaining free frames
    if (start != -1) {
        printf("                  %d-%d: Free frame(s)\n", start, memory->total_pages * page_size - 1);
    }
}
