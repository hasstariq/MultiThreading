#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>



void logStart(char* tID); //function to log that a new thread is started
void logFinish(char* tID); //function to log that a thread has finished its time

void startClock(); //function to start program clock
long getCurrentTime(); //function to check current time since clock was started
time_t programClock; //the global timer/clock for the program

typedef struct Thread //represents a single thread
{
	char tid[4];
    // st = start time, lt = life time, ct = current time
	int lt, st, b;

}Thread;

void* threadRun(void* t); //the thread function, the code executed by each thread

int readFile(char* fileName, Thread** threads)//use this method in a suitable way to read file
{
	FILE *in = fopen(fileName, "r");
	if(!in)
	{
		printf("Child A: Error in opening input file...exiting with error code -1\n");
		return -1;
	}

	struct stat st;
	fstat(fileno(in), &st);
	char* fileContent = (char*)malloc(((int)st.st_size+1)* sizeof(char));
	fileContent[0]='\0';
	while(!feof(in))
	{
		char line[100];
		if(fgets(line,100,in)!=NULL)
		{
			strncat(fileContent,line,strlen(line));
		}
	}
	fclose(in);

	char* command = NULL;
	int tc = 0;
	char* fileCopy = (char*)malloc((strlen(fileContent)+1)*sizeof(char));
	strcpy(fileCopy,fileContent);
	command = strtok(fileCopy,"\r\n");
	while(command!=NULL)
	{
		tc++;
		command = strtok(NULL,"\r\n");
	}

	threads = (Thread) malloc((sizeof(Thread)) * tc);

	char* lines[tc];
	command = NULL;
	int a=0;
	command = strtok(fileContent,"\r\n");
	while(command!=NULL)
	{
		lines[a] = malloc(sizeof(command)*sizeof(char));
		strcpy(lines[a],command);
		a++;
		command = strtok(NULL,"\r\n");
	}

	for(int k=0; k<tc; k++) {

		char* token = NULL;
		
		token =  strtok(lines[k],";");

        
		while(token!=NULL) { 
			strcpy((*threads)[k].tid, token);

			token = strtok (NULL, " ; ");
			
			(*threads)[k].st = atoi(token);

			token = strtok (NULL, " ; ");
			
			(*threads)[k].lt = atoi(token);

			token = strtok (NULL, " ; t ");
		}
	}

	return tc;
}

void logStart(char* tID)//invoke this method when you start a thread
{
	printf("[%ld] New Thread with ID %s is started.\n", getCurrentTime(), tID);
}

void logFinish(char* tID)//invoke this method when a thread is over
{
	printf("[%ld] Thread with ID %s is finished.\n", getCurrentTime(), tID);
}

void* threadRun(void* t)//implement this function in a suitable way
{

	Thread thread = (Thread)t;

	while ((thread->st + thread->lt) != getCurrentTime()) continue;

	logFinish(thread->tid);

	pthread_exit(0);
}

void startClock()//invoke this method when you start servicing threads
{
	programClock = time(NULL);
}

long getCurrentTime()//invoke this method whenever you want to check how much time units passed
{
	time_t now = time(NULL);
	return now-programClock;
}

int main(int argc, char *argv[]){
	
	if(argc<2) {
		printf("Input file name missing...exiting with error code -1\n");
		return -1;
	}

	Thread *my_threads;

	int tc = readFile(argv[1],&my_threads);

	startClock();
	pthread_t my_workers[tc]; 
	pthread_attr_t attr; 
	pthread_attr_init(&attr); 

    int a = 0;
	while(a < tc) //put a suitable condition here to run your program
	{
		int ts = -1;

		while (ts == -1){
            int b = 0;
			while(b < tc){
				long ct = getCurrentTime();

				if (my_threads[b].st == ct){
					logStart(my_threads[b].tid);
					pthread_create(&my_workers[b], &attr, threadRun, (void*) &my_threads[b]);
					ts = 1;

				} else if ((my_threads[b].st != ct))
					ts = -1;
			    b++;
            }
		}
		a++;
	}

    a = 0;
	while(a < tc) {
		pthread_join(my_workers[a], NULL);
        a++;
	}

	// exit threads
	pthread_attr_destroy(&attr);
	pthread_exit(NULL);
	free(my_threads);
	
