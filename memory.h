#ifndef MEMORY_H
#define MEMORY_H

typedef struct {
  int total_memory;  // Total size of memory in KB
  int page_size;     // Size of each page or chunk in KB
  int total_pages;   // Total number of pages in memory
  int *page_table;   // Array representing the allocation of pages (-1 for free,
                     // process ID for allocated)
} Memory;

// Function prototypes
void init_memory(Memory *memory, int total_memory, int page_size);
int allocate_memory(Memory *memory, int process_id, int num_pieces,
                    int *piece_sizes);
void deallocate_memory(Memory *memory, int process_id);
void print_memory_map(Memory *memory, int page_size);

#endif
