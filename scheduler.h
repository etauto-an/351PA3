#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "parser.h"

// A structure to represent a node in the input queue
typedef struct QueueNode {
  Process process;         // Process details
  struct QueueNode *next;  // Pointer to the next node in the queue
} QueueNode;

// A structure to represent the input queue
typedef struct {
  QueueNode *front;  // Pointer to the front of the queue
  QueueNode *rear;   // Pointer to the rear of the queue
} InputQueue;

// Function prototypes
void init_queue(InputQueue *queue);
void enqueue(InputQueue *queue, Process process);
Process dequeue(InputQueue *queue);
int is_queue_empty(InputQueue *queue);

#endif
