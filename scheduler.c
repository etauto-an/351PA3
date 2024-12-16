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
 *   - Sets the front and rear pointers of the queue to NULL, indicating an
 * empty queue.
 *   - This function must be called before performing any other queue
 * operations.
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
 *   - Dynamically allocates memory for a new queue node.
 *   - Inserts the new node at the end of the queue.
 *   - If the queue was empty, both front and rear pointers point to the new
 * node.
 *
 * Notes:
 *   - Exits the program with an error message if memory allocation fails.
 */
void enqueue(InputQueue *queue, Process process) {
  QueueNode *new_node = (QueueNode *)malloc(sizeof(QueueNode));
  if (!new_node) {
    perror("Error allocating memory for queue node");
    exit(EXIT_FAILURE);
  }
  new_node->process = process;  // Store the process in the new node
  new_node->next = NULL;

  if (queue->rear == NULL) {  // If the queue is empty
    queue->front = new_node;
    queue->rear = new_node;
  } else {  // Otherwise, add to the end of the queue
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
 *   - Removes the node at the front of the queue.
 *   - Returns the process stored in the removed node.
 *   - Updates the front pointer to point to the next node.
 *   - If the queue becomes empty after removal, sets the rear pointer to NULL.
 *
 * Notes:
 *   - Exits the program with an error message if an attempt is made to dequeue
 * from an empty queue.
 */
Process dequeue(InputQueue *queue) {
  if (queue->front == NULL) {
    fprintf(stderr, "Error: Attempt to dequeue from an empty queue.\n");
    exit(EXIT_FAILURE);
  }

  QueueNode *temp = queue->front;   // Temporary pointer to the front node
  Process process = temp->process;  // Retrieve the process from the front node

  queue->front = queue->front->next;  // Move the front pointer to the next node
  if (queue->front == NULL) {         // If the queue is now empty
    queue->rear = NULL;
  }

  free(temp);  // Free the memory of the removed node
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
 *
 * Behavior:
 *   - Returns true (1) if the front pointer is NULL, indicating the queue is
 * empty.
 *   - Returns false (0) otherwise.
 */
int is_queue_empty(InputQueue *queue) { return (queue->front == NULL); }
