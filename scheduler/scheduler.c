#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* header files */

/* global definitions */
struct proc {
	char name[10];
	int pid;
	int priority;
	int at; //Arrival Time
	int WT; //Waiting Time
	struct proc* next;
    struct proc* prev;
}typedef proc;

struct Queue {
    struct proc* head;
    struct proc* end;
}typedef queue;


/* definition and implementation of process descriptor and queue(s) */
void newProc(queue * q){
	proc* temp = (struct proc*)malloc(sizeof(struct proc));
	
	temp->next = NULL;
	if(q->end == NULL){
		q->head = temp;
		temp->prev = NULL;
	}else{
		q->end->next = temp;
		temp->prev = q->end;
	}
	q->end = temp;
}

// Creates an empty queue and returns a pointer to it.
struct Queue* createQueue() {
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
    q->head = q->end = NULL;
    return q;
}

// Removes the head key from the queue
struct proc* deQueue(struct Queue* q) {
    if (q->head == NULL)
        return NULL;
    struct proc* temp = q->head;
    if (q->head->next != NULL){
        q->head = q->head->next;
        q->head->prev = NULL;
    }
    else{
        q->head = NULL;
        q->end = NULL;
    }
    return temp;
}

void fill_queue (queue* q, FILE* fp) {

	int num;
	char str[20];
	while(fscanf(fp, "../work/ %s %d\n", str, &num)!=EOF){
		newProc(q);
		strcpy (q->end->name, str);
		q->end->at = num;
	}
}

void print(struct Queue* q){
	struct proc* ptr = q->head;
	while(ptr!=NULL){
		printf("%d\n", ptr->at);
		ptr = ptr->next;
	}

}
/* global variables and data structures */

/* signal handler(s) */

/* implementation of the scheduling policies, etc. batch(), rr() etc. */

int main(int argc,char **argv)
{
	/* local variables */

	/* parse input arguments (policy, quantum (if required), input filename */

	/* read input file - populate queue */
	struct Queue* queue1 = createQueue();

   	FILE * fp = fopen("homogeneous.txt", "r+");
	fill_queue(queue1, fp);
	print(queue1);
	
	
	
	
	fclose(fp);
   

	/* call selected scheduling policy */

	/* print information and statistics */

	printf("Scheduler exits\n");
	return 0;
}
