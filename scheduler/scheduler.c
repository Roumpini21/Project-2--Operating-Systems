#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
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

int count(struct Queue* q) {
    struct proc* temp = q->head;
    int count = 0;
    while(temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

void bubble(struct Queue* q){
	int f = 1;
    struct proc* ptr1 = q->head;
	proc* ptr2 = NULL;
	while (f!=0){
		f=0;
		ptr1=q->head;
		while (ptr1->next!=ptr2){
			if(ptr1->at > ptr1->next->at){
				int t = ptr1->at;
				char text[10];
				strcpy (text, ptr1->name);
				ptr1->at = ptr1->next->at;
				strcpy (ptr1->name, ptr1->next->name);
				ptr1->next->at = t;
				strcpy (ptr1->next->name, text);
				f=1;
			}
			ptr1=ptr1->next;
		}
		ptr2=ptr1;
	}
}

void fill_queue (queue* q, FILE* fp) {
	int num;
	char str[20];
	while(fscanf(fp, "../work/ %s %d\n", str, &num)!=EOF){
		newProc(q);
		strcpy (q->end->name, str);
		q->end->at = num;
	}
	bubble(q);
}

void print(struct Queue* q){
	struct proc* ptr = q->head;
	while(ptr!=NULL){
		printf("%s\n", ptr->name);
		ptr = ptr->next;
	}
}

/* global variables and data structures */

/* signal handler(s) */

/* implementation of the scheduling policies, etc. batch(), rr() etc. */

void batch(struct Queue* q){
	int c = count(q);
	double start_time, end_time;
    struct proc* current_proc;
    while (q->head != NULL) {
        current_proc = deQueue(q);
        start_time = time(NULL);
		char path[20] = "";
		strcat(path, "../work/");
		strcat(path, current_proc->name);
		printf("%s", path);
        pid_t pid = fork();
        if (pid == 0) {
			printf("HEYYYYY");
			current_proc->pid = getpid();
			pid_t pid1 = getpid();
			printf("%d", pid1);
            execl(path, current_proc->name, NULL);
        } else {
            wait(&current_proc->pid);
            end_time = time(NULL);
            double elapsed_time = difftime(end_time, start_time);
            printf("Process %d took %f seconds to finish.\n", current_proc->pid, elapsed_time);
        }
    }
}

int main(int argc,char **argv)
{
	/* local variables */

	/* parse input arguments (policy, quantum (if required), input filename */

	/* read input file - populate queue */
	struct Queue* queue1 = createQueue();

   	FILE * fp = fopen("homogeneous.txt", "r+");
	fill_queue(queue1, fp);
	print(queue1);
	batch(queue1);

	fclose(fp);
   

	/* call selected scheduling policy */

	/* print information and statistics */

	printf("Scheduler exits\n");
	return 0;
}
