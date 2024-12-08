#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "parser.h"
#include "scheduler.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Parse input file and initialize structures
  int num_processes;
  Process *processes = parse_input_file(argv[1], &num_processes);

  Memory memory;
  init_memory(&memory, 2000, 100);  // 2000 KB total memory, 100 KB page size

  InputQueue queue;
  init_queue(&queue);

  for (int i = 0; i < num_processes; i++) {
    enqueue(&queue, processes[i]);
  }

  int clock = 0;
  double total_turnaround = 0;
  int completed_processes = 0;

  while (clock <= 100000) {
    int event_occurred = 0;

    // Check for process completions
    for (int i = 0; i < num_processes; i++) {
      Process *process = &processes[i];
      int completion_time = process->arrival_time + process->lifetime;

      if (completion_time == clock) {
        if (!event_occurred) {
          printf("t = %d: ", clock);
          event_occurred = 1;
        }
        printf("Process %d completes\n", process->id);
        deallocate_memory(&memory, process->id);
        total_turnaround += (clock - process->arrival_time);
        completed_processes++;
      }
    }

    // Check for process arrivals
    if (!is_queue_empty(&queue)) {
      Process next_process = queue.front->process;

      if (next_process.arrival_time == clock) {
        if (!event_occurred) {
          printf("t = %d: ", clock);
          event_occurred = 1;
        }
        printf("Process %d arrives\n", next_process.id);

        // Print input queue state
        printf("       Input Queue:[");
        QueueNode *node = queue.front;
        while (node) {
          printf("%d ", node->process.id);
          node = node->next;
        }
        printf("]\n");

        // Try to admit the process into memory
        if (allocate_memory(&memory, next_process.id,
                            next_process.memory_pieces,
                            next_process.piece_sizes)) {
          printf("       MM moves Process %d to memory\n", next_process.id);
          dequeue(&queue);
        }
      }
    }

    // Print the memory map only when an event occurs
    if (event_occurred) {
      print_memory_map(&memory, 100);
    }

    // Check if the simulation can end
    int is_memory_empty = 1;
    for (int i = 0; i < memory.total_pages; i++) {
      if (memory.page_table[i] != -1) {
        is_memory_empty = 0;
        break;
      }
    }
    if (is_queue_empty(&queue) && is_memory_empty) {
      break;
    }

    clock++;
  }

  printf("Simulation ends at time %d.\n", clock);
  printf("Average Turnaround Time: %.2f\n",
         total_turnaround / completed_processes);

  // Clean up
  free(memory.page_table);
  free_parsed_data(processes, num_processes);

  return EXIT_SUCCESS;
}
