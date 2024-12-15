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
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Parse input file and initialize structures
  int num_processes;
  Process *processes = parse_input_file(argv[1], &num_processes);

  Memory memory;
  init_memory(&memory, 2000, 400);

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
          printf("t = %d:\n", clock);
          event_occurred = 1;
        }
        enqueue(&queue, processes[i]);
        printf("       Process %d arrives\n", processes[i].id);

        // Print input queue state
        print_input_queue(&queue);
      }
    }

    // Check for process completions
    for (int i = 0; i < num_processes; i++) {
      Process *process = &processes[i];
      int completion_time = process->arrival_time + process->lifetime;

      if (completion_time == clock) {
        if (!event_occurred) {
          printf("t = %d:\n", clock);
          event_occurred = 1;
        }
        printf("       Process %d completes\n", process->id);
        deallocate_memory(&memory, process->id);

        // Print memory map after deallocation
        print_memory_map(&memory, memory.page_size);
        printf("\n");

        total_turnaround += (clock - process->arrival_time);
        completed_processes++;

      }
    }

    // Check for process arrivals from the queue
    while (!is_queue_empty(&queue)) {
      Process next_process = queue.front->process;

      if (allocate_memory(&memory, next_process.id, next_process.memory_pieces, next_process.piece_sizes)) {
        if (!event_occurred) {
          printf("t = %d:\n", clock);
          event_occurred = 1;
        }
        dequeue(&queue);
        printf("       MM moves Process %d to memory\n", next_process.id);

        // Print input queue state
        print_input_queue(&queue);

        // Print memory map after allocation
        print_memory_map(&memory, memory.page_size);
        printf("\n");
      } else {
        // Break the loop if the process cannot be allocated
        break;
      }
    }

    // Increment clock only if an event occurred
    if (event_occurred) {
      clock++;
    } else {
      clock++;
    }
  }

  printf("Average Turnaround Time: %.2f\n", total_turnaround / completed_processes);
  return EXIT_SUCCESS;
}
