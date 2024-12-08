#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Parses the input file to extract process details.
 *
 * Args:
 *   filename (const char*): Path to the input file containing process details.
 *   num_processes (int*): Pointer to an integer where the total number of
 * processes will be stored.
 *
 * Returns:
 *   Process*: Pointer to an array of `Process` structures containing the parsed
 * process details. The caller is responsible for freeing the allocated memory
 * using `free_parsed_data`.
 *
 * Behavior:
 *   - Reads the input file and parses process information including:
 *     - Process ID.
 *     - Arrival time.
 *     - Lifetime.
 *     - Number of memory pieces.
 *     - Sizes of each memory piece.
 *   - Allocates memory dynamically for the process array and memory piece
 * sizes.
 *
 * Errors:
 *   - Exits the program with an error message if:
 *     - The file cannot be opened.
 *     - Memory allocation fails.
 */
Process *parse_input_file(const char *filename, int *num_processes) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // Read the number of processes
  fscanf(file, "%d", num_processes);

  // Allocate memory for the processes
  Process *processes = malloc((*num_processes) * sizeof(Process));
  if (!processes) {
    perror("Error allocating memory for processes");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  // Read each process's details
  for (int i = 0; i < *num_processes; i++) {
    // Read process ID
    fscanf(file, "%d", &processes[i].id);

    // Read arrival time and lifetime
    fscanf(file, "%d %d", &processes[i].arrival_time, &processes[i].lifetime);

    // Read number of memory pieces
    fscanf(file, "%d", &processes[i].memory_pieces);

    // Allocate memory for the piece sizes
    processes[i].piece_sizes = malloc(processes[i].memory_pieces * sizeof(int));
    if (!processes[i].piece_sizes) {
      perror("Error allocating memory for piece sizes");
      fclose(file);
      exit(EXIT_FAILURE);
    }

    // Read the sizes of the memory pieces
    for (int j = 0; j < processes[i].memory_pieces; j++) {
      fscanf(file, "%d", &processes[i].piece_sizes[j]);
    }
  }

  fclose(file);
  return processes;
}

/**
 * Frees the memory allocated for the parsed process data.
 *
 * Args:
 *   processes (Process*): Pointer to the array of `Process` structures.
 *   num_processes (int): Number of processes in the array.
 *
 * Behavior:
 *   - Frees the dynamically allocated memory for:
 *     - Each process's `piece_sizes` array.
 *     - The array of `Process` structures itself.
 *
 * Notes:
 *   - This function should be called to avoid memory leaks after using the
 * parsed data.
 */
void free_parsed_data(Process *processes, int num_processes) {
  for (int i = 0; i < num_processes; i++) {
    free(processes[i].piece_sizes);
  }
  free(processes);
}
