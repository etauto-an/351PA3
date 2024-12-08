#include "scheduler.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Initializes the input queue.
 *
 * Args:
 *   queue (InputQueue*): Pointer to the input queue structure.
 *
 * Behavior:
 *   - Sets the front and rear pointers to NULL.
 */
void init_queue(InputQueue *queue) {
  queue->front = NULL;
  queue->rear = NULL;
}

/**
 * Adds a process to the input queue.
 *
 * Args:
 *   queue (InputQueue*): Pointer to the input queue structure.
 *   process (Process): Process to be added to the queue.
 *
 * Behavior:
 *   - Creates a new node for the process.
 *   - Adds the node to the end of the queue.
 */
void enqueue(InputQueue *queue, Process process) {
  QueueNode *new_node = (QueueNode *)malloc(sizeof(QueueNode));
  if (!new_node) {
    perror("Error allocating memory for queue node");
    exit(EXIT_FAILURE);
  }
  new_node->process = process;
  new_node->next = NULL;

  if (queue->rear == NULL) {  // If the queue is empty
    queue->front = new_node;
    queue->rear = new_node;
  } else {  // Add to the end of the queue
    queue->rear->next = new_node;
    queue->rear = new_node;
  }
}

/**
 * Removes a process from the front of the input queue.
 *
 * Args:
 *   queue (InputQueue*): Pointer to the input queue structure.
 *
 * Returns:
 *   Process: The process at the front of the queue.
 *
 * Behavior:
 *   - Removes the front node from the queue and returns its process data.
 *   - If the queue is empty, exits the program with an error.
 */
Process dequeue(InputQueue *queue) {
  if (queue->front == NULL) {
    fprintf(stderr, "Error: Attempt to dequeue from an empty queue.\n");
    exit(EXIT_FAILURE);
  }

  QueueNode *temp = queue->front;
  Process process = temp->process;

  queue->front = queue->front->next;
  if (queue->front == NULL) {  // If the queue is now empty
    queue->rear = NULL;
  }

  free(temp);
  return process;
}

/**
 * Checks if the input queue is empty.
 *
 * Args:
 *   queue (InputQueue*): Pointer to the input queue structure.
 *
 * Returns:
 *   int: 1 if the queue is empty, 0 otherwise.
 */
int is_queue_empty(InputQueue *queue) { return (queue->front == NULL); }
