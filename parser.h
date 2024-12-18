#ifndef PARSER_H
#define PARSER_H

typedef struct {
  int id;             // Process ID
  int arrival_time;   // Arrival time
  int lifetime;       // Time the process stays in memory
  int memory_pieces;  // Number of memory segments
  int *piece_sizes;   // Array of memory segment sizes
  int start_time;  // The time when the process is first moved to memory (-1 if
                   // not started yet)
} Process;

// Function prototypes
Process *parse_input_file(const char *filename, int *num_processes);
void free_parsed_data(Process *processes, int num_processes);

#endif
