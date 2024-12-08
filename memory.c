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
int allocate_memory(Memory *memory, int process_id, int num_pieces,
                    int *piece_sizes) {
  int pages_needed, free_count, start;

  for (int i = 0; i < num_pieces; i++) {
    pages_needed = (piece_sizes[i] + memory->page_size - 1) /
                   memory->page_size;  // Round up
    free_count = 0;
    start = -1;

    // Find a block of free pages
    for (int j = 0; j < memory->total_pages; j++) {
      if (memory->page_table[j] == -1) {  // Free page
        if (free_count == 0) start = j;
        free_count++;
        if (free_count == pages_needed) break;
      } else {
        free_count = 0;
      }
    }

    // If enough pages are found, allocate them
    if (free_count >= pages_needed) {
      for (int j = start; j < start + pages_needed; j++) {
        memory->page_table[j] = process_id;
      }
    } else {
      // Not enough memory; roll back previous allocations
      for (int k = 0; k < memory->total_pages; k++) {
        if (memory->page_table[k] == process_id) {
          memory->page_table[k] = -1;
        }
      }
      return 0;  // Allocation failed
    }
  }

  return 1;  // Allocation successful
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

/**
 * Prints the current memory map.
 *
 * Args:
 *   memory (Memory*): Pointer to the `Memory` structure.
 *
 * Behavior:
 *   - Displays the allocation status of each page in memory.
 *   - Each page is either marked as "Free" or assigned to a process ID.
 */
void print_memory_map(Memory *memory, int page_size) {
  int start = -1;  // Start of a free range
  int current_process = -1;

  printf("       Memory Map:\n");

  for (int i = 0; i < memory->total_pages; i++) {
    if (memory->page_table[i] == -1) {  // Free page
      if (start == -1) {
        start = i;  // Mark the start of a free range
      }
    } else {
      if (start != -1) {  // End of a free range
        printf("       %d-%d: Free frame(s)\n", start * page_size,
               (i * page_size) - 1);
        start = -1;  // Reset start
      }
      if (current_process != memory->page_table[i]) {
        current_process = memory->page_table[i];
        printf("       %d-%d: Process %d, Page %d\n", i * page_size,
               (i + 1) * page_size - 1, current_process, i + 1);
      }
    }
  }

  // If free frames extend to the end of memory, print the final range
  if (start != -1) {
    printf("       %d-%d: Free frame(s)\n", start * page_size,
           (memory->total_pages * page_size) - 1);
  }
}
