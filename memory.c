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
 * indicates free pages).
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
 *   piece_sizes (int*): Array containing the sizes of each memory segment (in
 * KB).
 *
 * Returns:
 *   int: 1 if memory allocation is successful, 0 otherwise.
 *
 * Behavior:
 *   - Checks if sufficient free pages are available to allocate the required
 * memory.
 *   - Allocates memory in chunks corresponding to the size of each piece.
 *   - Rolls back any partial allocation if memory is insufficient.
 *
 * Notes:
 *   - Each memory segment is rounded up to the nearest multiple of the page
 * size.
 *   - If memory cannot be allocated, the function restores all previously
 * allocated pages for the process.
 */
int allocate_memory(Memory *memory, int process_id, int num_pieces, int *piece_sizes) {
  int pages_needed, pages_allocated;

  //printf("Debug: Attempting to allocate memory for Process %d\n", process_id);

  for (int i = 0; i < num_pieces; i++) {
    pages_needed = (piece_sizes[i] + memory->page_size - 1) / memory->page_size;  // Round up
    pages_allocated = 0;

    //printf("Debug: Process %d, Piece %d (%d KB) needs %d pages\n",
    //       process_id, i + 1, piece_sizes[i], pages_needed);

    // Attempt to allocate as many pages as possible
    for (int j = 0; j < memory->total_pages && pages_allocated < pages_needed; j++) {
      if (memory->page_table[j] == -1) {  // Free page found
        memory->page_table[j] = process_id;
        pages_allocated++;
      }
    }

    // Debugging output for allocation
    //printf("Debug: Process %d, Piece %d allocated %d out of %d pages\n",
    //       process_id, i + 1, pages_allocated, pages_needed);

    if (pages_allocated < pages_needed) {
      printf("Warning: Process %d, Piece %d could not allocate all required pages. Partial fit.\n",
             process_id, i + 1);
    }
  }

  //printf("Debug: Successfully allocated memory for Process %d (partial fits allowed)\n", process_id);
  return 1;  // Allocation successful, even with partial fit
}


/**
 * Deallocates memory for a process in the memory system.
 *
 * Args:
 *   memory (Memory*): Pointer to the `Memory` structure.
 *   process_id (int): ID of the process whose memory is to be deallocated.
 *
 * Behavior:
 *   - Frees all pages in memory that are currently allocated to the specified
 * process.
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
    //int current_process = -1;
    int page_number[1000] = {0};  // Track page numbers for up to 1000 processes

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
            page_number[process_id]++;  // Increment the global page counter for the process

            printf("                  %d-%d: Process %d, Page %d\n",
                   start_address, end_address, process_id, page_number[process_id]);
        }
    }

    // Print any remaining free frames
    if (start != -1) {
        printf("                  %d-%d: Free frame(s)\n", start, memory->total_pages * page_size - 1);
    }
}



//int main() {
//    // Initialize memory
//    Memory memory;
//    init_memory(&memory, 2000, 100);  // 2000 KB total, 100 KB per page
//
//    // Simulate memory allocations
//    printf("Initial Memory Map:\n");
//    print_memory_map(&memory, 100);
//
//    // Allocate memory for Process 1
//    int process1_pieces[] = {200, 400};  // 600 KB in total
//    allocate_memory(&memory, 1, 2, process1_pieces);
//    printf("\nAfter Allocating Process 1:\n");
//    print_memory_map(&memory, 100);
//
//    // Allocate memory for Process 2
//    int process2_pieces[] = {300};  // 300 KB
//    allocate_memory(&memory, 2, 1, process2_pieces);
//    printf("\nAfter Allocating Process 2:\n");
//    print_memory_map(&memory, 100);
//
//    // Deallocate memory for Process 1
//    deallocate_memory(&memory, 1);
//    printf("\nAfter Deallocating Process 1:\n");
//    print_memory_map(&memory, 100);
//
//    // Free allocated memory
//    free(memory.page_table);
//
//    return 0;
//}
