#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

/* global definitions */
struct proc {
	char name[10];
	int pid;
	int priority;
	int at; //Arrival Time
	int bt; //Burst Time
	float wt; //Work Time
	struct proc* next;
    struct proc* prev;
	char state[10];
	int status;
}typedef proc;

struct Queue {
    struct proc* head;
    struct proc* end;
}typedef queue;

proc *process;
int count = 0;

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

void print(queue* q){
	struct proc* ptr = q->head;
	while(ptr!=NULL){
		printf("%s\n", ptr->state);
		ptr = ptr->next;
	}
	count++;
	printf("%d------------------------------------------\n", count);
}

// Removes the head key from the queue
struct proc* deQueue(queue* q) {
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

void enqueue(queue *q, proc *proc) {
    proc->next = NULL;
    proc->prev = q->end;
    if(q->end == NULL) {
        q->head = q->end = proc;
        return;
    }
    q->end->next = proc;
    q->end = proc;
}

void bubble_batch(queue* q){
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

void bubble_sjf(queue* q){
	int f = 1;
    struct proc* ptr1 = q->head;
	proc* ptr2 = NULL;
	while (f!=0){
		f=0;
		ptr1=q->head;
		while (ptr1->next!=ptr2){
			if(ptr1->bt > ptr1->next->bt){
				int t = ptr1->bt;
				char text[10];
				strcpy (text, ptr1->name);
				ptr1->bt = ptr1->next->bt;
				strcpy(ptr1->state, ptr1->next->state);
				strcpy (ptr1->name, ptr1->next->name);
				ptr1->next->bt = t;
				strcpy (ptr1->next->name, text);
				f=1;
			}
			ptr1=ptr1->next;
		}
		ptr2=ptr1;
	}
}

void fill_queue (queue* q, FILE* fp, int option) {
	int num;
	char str[20];
	switch(option){
		case 1:
			while(fscanf(fp, "../work/ %s %d\n", str, &num)!=EOF){
				newProc(q);
				strcpy (q->end->name, str);
				q->end->at = num;
				strcpy(q->end->state, "READY");
			}
			bubble_batch(q);
			break;
		case 2:
			while(fscanf(fp, "../work/ %s %d\n", str, &num)!=EOF){
				newProc(q);
				strcpy (q->end->name, str);
				q->end->bt = num;
				strcpy(q->end->state, "READY");
			}
			bubble_sjf(q);
			break;
		case 3:
			while(fscanf(fp, "../work/ %s %d\n", str, &num)!=EOF){
				newProc(q);
				strcpy (q->end->name, str);
				q->end->priority = num;
				strcpy(q->end->state, "READY");
			}
			break;
	}
}

/* signal handler(s) */

void childHandler(int signum) {
	if(process->pid == waitpid(process->pid, &process->status, WNOHANG)){
		strcpy(process->state, "EXITED");
	}
}

void batch_sjf(queue* q){
	struct timespec start_time, end_time;
	float temp_time = 0;
    struct proc* current_proc;
    while (q->head != NULL){
        current_proc = deQueue(q);
        clock_gettime(CLOCK_MONOTONIC, &start_time);
		char path[20] = "";
		strcat(path, "../work/");
		strcat(path, current_proc->name);
		int pid = fork();
        if (pid == 0){
			strcpy(current_proc->state, "RUNNING");
            execl(path, current_proc->name, NULL);
        }else{
			current_proc->pid = pid;
            wait(NULL);
			strcpy(current_proc->state, "EXITED");
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;
			float temp_time = temp_time + elapsed_time;
			current_proc->wt = temp_time;
            printf("PID %d - CMD: %s \n\t\t Elapsed time: %.3f secs \n\t\t Workload Time: %.3f secs.\n", current_proc->pid, current_proc->name, elapsed_time, current_proc->wt);
        }
    }
	printf("WORKLOAD TIME: %.3f seconds\n", current_proc->wt);
}

void round_robin(queue *q, int quantum){
	signal(SIGCHLD, childHandler);
	struct timespec tim, tim2;
	tim.tv_sec = quantum;
   	tim.tv_nsec = 0;
	struct proc* current_proc;
	char path[20] = "";
    while(q->head != NULL){
		current_proc = deQueue(q);
		process = current_proc;
		strcpy(path, "../work/");
		strcat(path, current_proc->name);
		if(!strcmp(current_proc->state, "STOPPED")){
			strcpy(current_proc->state, "RUNNING");
			if(!strcmp(current_proc->state, "RUNNING")){
				printf("Hi from process %d\n", current_proc->pid);
				print(q);
				/*kill(current_proc->pid, SIGCONT);
				nanosleep(&tim, &tim2);
				kill(current_proc->pid, SIGSTOP);
				strcpy(current_proc->state, "STOPPED");
				enqueue(q, current_proc);*/
			}
		}else{
			int pid = fork();
			if(pid == 0){
				execl(path, current_proc->name, NULL);
			}else{
				strcpy(current_proc->state, "RUNNING");
				current_proc->pid = pid;
				nanosleep(&tim, &tim2);
				kill(current_proc->pid, SIGSTOP);
				strcpy(current_proc->state, "STOPPED");
				enqueue(q, current_proc);
			}
		}
	}
}

int main(int argc, char **argv){
	/* local variables */
	struct Queue* queue1 = createQueue();
	FILE * fp;
	int option = 0;
	/* parse input arguments (policy, quantum (if required), input filename */
	if (argv[3] != NULL){fp = fopen(argv[3], "r+");}
	else {fp = fopen(argv[2], "r+");}
	/* read input file - populate queue */
	
	if(!strcmp(argv[1], "BATCH")){
		option = 1;
		printf("Batch Algorithm Selected.\n");
		fill_queue(queue1, fp, 1);
		//print(queue1);
		batch_sjf(queue1);
	}else if(!strcmp(argv[1], "SJF")){
		option = 2;
		printf("SJF Algorithm Selected.\n");
		fill_queue(queue1, fp, 2);
		//print(queue1);
		batch_sjf(queue1);
	}else if(!strcmp(argv[1], "RR")){
		int quantum;
		option = 3;
		printf("RR Algorithm Selected.\n");
		fill_queue(queue1, fp, 3);
		print(queue1);
		round_robin(queue1, (atoi(argv[2])/1000));
	}else if(!strcmp(argv[1], "PRIO")){
		printf("PRIO Algorithm Selected.\n");
	}else{printf("Error Occured.");}

	fclose(fp);
	printf("Scheduler exits\n");
	return 0;
}