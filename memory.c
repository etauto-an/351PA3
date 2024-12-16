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
 *   memory (Memory*): Pointer to the `Memory` structure representing the memory
 * system. process_id (int): The ID of the process requesting memory allocation.
 *   num_pieces (int): Number of memory segments (pieces) required by the
 * process. piece_sizes (int*): Array containing the sizes of each memory
 * segment (in KB).
 *
 * Returns:
 *   int: 1 if memory allocation is successful, 0 if insufficient memory or
 * allocation fails.
 *
 * Behavior:
 *   1. Calculates the total number of pages needed for all requested memory
 * segments, rounding up each segment's size to the nearest page boundary.
 *   2. Counts the total number of free pages in the memory.
 *   3. If the total free pages are insufficient to meet the process's
 * requirements, the function immediately returns 0, indicating failure.
 *   4. If enough free pages are available:
 *      - Allocates memory segment-by-segment by marking free pages in the page
 * table.
 *      - If at any point a segment cannot be fully allocated, all previously
 * allocated pages for this process are rolled back (freed), and the function
 * returns 0.
 *   5. If all segments are successfully allocated, the function returns 1.
 *
 * Notes:
 *   - Memory allocation follows a "first fit" approach, scanning the page table
 *     sequentially for free pages.
 *   - Rollback ensures consistency in the event of partial allocation failure.
 */
int allocate_memory(Memory *memory, int process_id, int num_pieces,
                    int *piece_sizes) {
  int total_pages_needed = 0;
  for (int i = 0; i < num_pieces; i++) {
    int pages_for_piece =
        (piece_sizes[i] + memory->page_size - 1) / memory->page_size;
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
    return 0;  // Not enough memory, must wait
  }

  // Enough memory is available, proceed to allocate
  for (int i = 0; i < num_pieces; i++) {
    int pages_needed =
        (piece_sizes[i] + memory->page_size - 1) / memory->page_size;
    int pages_allocated = 0;

    for (int j = 0; j < memory->total_pages && pages_allocated < pages_needed;
         j++) {
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
      return 0;  // Fail allocation
    }
  }

  return 1;  // Allocation successful
}

/**
 * Deallocates memory for a process in the memory system.
 *
 * Args:
 *   memory (Memory*): Pointer to the `Memory` structure representing the memory
 * system. process_id (int): ID of the process whose allocated memory is to be
 * freed.
 *
 * Behavior:
 *   - Iterates through the memory's page table.
 *   - Identifies all pages currently allocated to the specified process
 * (matching `process_id`).
 *   - Marks those pages as free by setting their entries in the page table to
 * -1.
 *
 * Notes:
 *   - This function assumes that `process_id` corresponds to a valid process.
 */
void deallocate_memory(Memory *memory, int process_id) {
  // Loop through all pages in the memory system
  for (int i = 0; i < memory->total_pages; i++) {
    // If the page belongs to the specified process, mark it as free
    if (memory->page_table[i] == process_id) {
      memory->page_table[i] = -1;  // Mark page as free by setting it to -1
    }
  }
}

/**
 * Prints the current memory map, showing free frames and allocated pages.
 *
 * Args:
 *   memory (Memory*): Pointer to the `Memory` structure containing the memory
 * state. page_size (int): Size of each page in the memory system (in KB).
 *
 * Behavior:
 *   - Iterates through the memory's page table.
 *   - Prints ranges of free frames as contiguous blocks.
 *   - Prints allocated frames with process ID and corresponding page number.
 *   - Tracks page numbers for each process to display correct page numbering.
 *
 * Notes:
 *   - The function assumes memory is divided into fixed-size pages.
 *   - Free frames are identified with a `-1` in the page table.
 */
void print_memory_map(Memory *memory, int page_size) {
  printf("       Memory Map:\n");

  int start = -1;  // Marks the start address of a free range
  int page_number[1000] = {
      0};  // Tracks the page numbers for processes (up to 1000 process IDs)

  // Iterate through all pages in the memory
  for (int i = 0; i < memory->total_pages; i++) {
    int start_address = i * page_size;  // Start address of the current page
    int end_address =
        (i + 1) * page_size - 1;  // End address of the current page

    // Check if the current page is free
    if (memory->page_table[i] == -1) {
      if (start == -1) start = start_address;  // Mark the start of a free range
    } else {
      // If a free range was being tracked, print it
      if (start != -1) {
        printf("                  %d-%d: Free frame(s)\n", start,
               start_address - 1);
        start = -1;  // Reset the start of the free range
      }

      // Print allocated page details
      int process_id = memory->page_table[i];
      page_number[process_id]++;  // Increment the page count for the process

      printf("                  %d-%d: Process %d, Page %d\n", start_address,
             end_address, process_id, page_number[process_id]);
    }
  }

  // If we have an ongoing free range at the end of the memory, print it
  if (start != -1) {
    printf("                  %d-%d: Free frame(s)\n", start,
           memory->total_pages * page_size - 1);
  }
}
