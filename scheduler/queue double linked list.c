#include <stdio.h>
#include <stdlib.h>

struct Queue {
    struct proc* front;
    struct proc* rear;
};

// Creates a new proc and returns pointer to it.
struct proc* newNode(int key) {
    struct proc* temp = (struct proc*)malloc(sizeof(struct proc));
    temp->data = key;
    temp->prev = NULL;
    temp->next = NULL;
    return temp;
}

// Creates an empty queue and returns a pointer to it.
struct Queue* createQueue() {
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

// Adds a new key to the rear of the queue
void enQueue(struct Queue* q, int key) {
    struct proc* temp = newNode(key);
    if(q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }
    temp->prev = q->rear;
    q->rear = temp;
}

// Removes the front key from the queue
struct proc* deQueue(struct Queue* q) {
    if (q->front == NULL)
        return NULL;
    struct proc* temp = q->front;
    if (q->front->next != NULL) {
        q->front = q->front->next;
        q->front->prev = NULL;
    }
    else {
        q->front = NULL;
        q->rear = NULL;
    }
    return temp;
}