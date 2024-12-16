#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "parser.h"
#include "scheduler.h"

// Function to print the current state of the input queue
void print_input_queue(InputQueue *queue) {
  printf("       Input Queue:[");
  QueueNode *node = queue->front;
  while (node) {
    printf("%d", node->process.id);
    if (node->next) printf(" ");
    node = node->next;
  }
  printf("]\n");
}

int main(int argc, char *argv[]) {
  if (argc <
      4) {  // Ensure there are 3 arguments: input_file, total_memory, page_size
    fprintf(stderr, "Usage: %s <input_file> <total_memory_size> <page_size>\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  // Parse command-line arguments
  const char *input_file = argv[1];
  int total_memory = atoi(argv[2]);
  int page_size = atoi(argv[3]);

  if (total_memory <= 0 || page_size <= 0 || total_memory % page_size != 0) {
    fprintf(stderr,
            "Error: Invalid memory size or page size. Ensure total_memory > 0, "
            "page_size > 0, and total_memory is divisible by page_size.\n");
    return EXIT_FAILURE;
  }

  // Parse input file and initialize structures
  int num_processes;
  Process *processes = parse_input_file(input_file, &num_processes);

  // Explicitly initialize start_time to -1 for all processes
  for (int i = 0; i < num_processes; i++) {
    processes[i].start_time =
        -1;  // Ensure all start times are marked as "not started"
  }

  // Initialize memory with total size and page size from arguments
  Memory memory;
  init_memory(&memory, total_memory, page_size);

  InputQueue queue;
  init_queue(&queue);

  int clock = 0;
  double total_turnaround = 0;
  int completed_processes = 0;

  while (clock <= 100000) {
    int event_occurred = 0;

    // Dynamically enqueue processes based on arrival time
    for (int i = 0; i < num_processes; i++) {
      if (processes[i].arrival_time == clock) {
        if (!event_occurred) {
          printf("\nt = %d:\n", clock);
          event_occurred = 1;
        }
        enqueue(&queue, processes[i]);
        printf("       Process %d arrives\n", processes[i].id);

        // Print input queue state
        print_input_queue(&queue);
      }
    }

    // Check for process completions (FCFS order for same completion time)
    for (int i = 0; i < num_processes; i++) {
      Process *process = &processes[i];

      // Only consider completion if the process has actually started
      if (process->start_time != -1) {
        int completion_time = process->start_time + process->lifetime;
        if (completion_time == clock) {
          if (!event_occurred) {
            printf("\nt = %d:\n", clock);
            event_occurred = 1;
          }
          printf("       Process %d completes\n", process->id);
          deallocate_memory(&memory, process->id);

          // Print memory map after deallocation
          print_memory_map(&memory, memory.page_size);

          total_turnaround += (clock - process->arrival_time);
          completed_processes++;
        }
      }
    }

    // Attempt to allocate memory for processes in the queue
    while (!is_queue_empty(&queue)) {
      Process next_process = queue.front->process;

      // Allocate memory if possible
      if (allocate_memory(&memory, next_process.id, next_process.memory_pieces,
                          next_process.piece_sizes)) {
        // Mark the start time for this process
        for (int p = 0; p < num_processes; p++) {
          if (processes[p].id == next_process.id) {
            processes[p].start_time = clock;  // Process starts now
            break;
          }
        }

        if (!event_occurred) {
          printf("\nt = %d:\n", clock);
          event_occurred = 1;
        }
        dequeue(&queue);
        printf("       MM moves Process %d to memory\n", next_process.id);

        // Print input queue state
        print_input_queue(&queue);

        // Print memory map after allocation
        print_memory_map(&memory, memory.page_size);

      } else {
        // Cannot allocate the next process yet, break to move time forward
        break;
      }
    }

    // Increment clock
    clock++;
  }

  // Calculate and print the average turnaround time
  if (completed_processes > 0) {
    printf("\nAverage Turnaround Time: %.2f\n",
           total_turnaround / completed_processes);
  } else {
    printf("No processes completed. Average Turnaround Time: N/A\n");
  }

  // Free dynamically allocated memory
  free_parsed_data(processes, num_processes);
  free(memory.page_table);

  return EXIT_SUCCESS;
}
