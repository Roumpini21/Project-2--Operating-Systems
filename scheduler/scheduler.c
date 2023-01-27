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
	float WT; //Waiting Time
	struct proc* next;
    struct proc* prev;
	char state[10];
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
		printf("%d\n", ptr->at);
		ptr = ptr->next;
	}
}

/* global variables and data structures */

/* signal handler(s) */

/* implementation of the scheduling policies, etc. batch(), rr() etc. */

void batch(struct Queue* q){
	int c = count(q);
	struct timespec start_time, end_time;
    struct proc* current_proc;
    while (q->head != NULL) {
        current_proc = deQueue(q);
        clock_gettime(CLOCK_MONOTONIC, &start_time);
		char path[20] = "";
		strcpy(current_proc->state, "READY");
		strcat(path, "../work/");
		strcat(path, current_proc->name);
		int pid = fork();
        if (pid == 0){
			strcpy(current_proc->state, "RUNNING");
            execl(path, current_proc->name, NULL);
        } else {
			current_proc->pid = pid;
            wait(NULL);
			strcpy(current_proc->state, "EXITED");
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;
			float temp_time = temp_time + elapsed_time;
			current_proc->WT = temp_time;
            printf("PID %d - CMD: %s \n\t\t Elapsed time: %.3f secs \n\t\t Workload Time: %.3f secs.\n", current_proc->pid, current_proc->name, elapsed_time, current_proc->WT);
        }
    }
	printf("WORKLOAD TIME: %.3f seconds\n", current_proc->WT);
}

int main(int argc,char **argv)
{
	/* local variables */

	/* parse input arguments (policy, quantum (if required), input filename */

	/* read input file - populate queue */
	struct Queue* queue1 = createQueue();
	FILE * fp;
	if (argv[3] != NULL){fp = fopen(argv[3], "r+");}
	else {fp = fopen(argv[2], "r+");}

	if(!strcmp(argv[1], "BATCH")){
		printf("Batch Algorithm Selected.");
		fill_queue(queue1, fp);
		//print(queue1);
		batch(queue1);
	}else if(!strcmp(argv[1], "SJF")){
		printf("SJF Algorithm Selected.");
	}else if(!strcmp(argv[1], "RR")){
		printf("RR Algorithm Selected.");
	}else if(!strcmp(argv[1], "PRIO")){
		printf("PRIO Algorithm Selected.");
	}else{printf("Error Occured.");}

	fclose(fp);
   

	/* call selected scheduling policy */

	/* print information and statistics */

	printf("Scheduler exits\n");
	return 0;
}
