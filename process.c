/*
Name: Andrew Burcroff
Class: Operating Systems
Assignments: semaphores Threads

Details: Sets up deadlock conditions for practice detecting and recovering.
There
will be to semaphores needed screen, keyboard (NAMED or Unamed semaphores).
Algorithm: Mother process called 'main' will open both semaphores while spanning
9 threads then they will die. The function getSemaphores returns
deadlock_count_number of how
many times recovered from deadlock. Finally the process, the threads will call
this process to prompt the user and deadlock_count_number number of deadlocks

Reference: http://man7.org/linux/man-pages/man7/sem_overview.7.html
*/
#include <errno.h>
//#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

struct semaphores{
  sem_t *screen_semaphore;         //Created a semaphore called for a screen
  sem_t *keyboard_semaphore;
  sem_t* counter;
  int id;
};

int lock_two_sems(sem_t* a, sem_t* b){
	int deadlock_count_number = 2;//create deadlock_count_number.
	int both_semphores = 0;//create varable of both_semphores
	int timer;//create of timer
	struct timespec struct_time;//struct of timespec
	while(both_semphores != 1){
			if(sem_wait(a) == -1){
				perror("ERROR: sem_wait for Screen failed");//ERROR message
			}
			if(clock_gettime(CLOCK_REALTIME, &struct_time) == -1) {
				perror("ERROR: Gettime for the clock has failed");//ERROR message
			}
			struct_time.tv_sec += 2;//set the struct_time increase
			if((timer = sem_timedwait(b, &struct_time)) == -1){
				if(sem_post(a) == -1) {
					perror("ERROR: sem_post for Screen Semaphore has failed");//ERROR message
				}
        else {
					//printf("had to unlonk a!\n");
				}
				deadlock_count_number++;//increase deadlock_count_number
        srand(time(NULL));//random time to null
        usleep((rand() % 100) +1);//sleep for random time
			}
      else if(timer == -1) {
				perror("ERROR: sem_timewait for Screen Semaphore failed");//ERROR message
			}
      else {
				both_semphores = 1;//set both_semphores to 1
			}  // end of else
	}
	return deadlock_count_number;//return deadlock_count_number
}

#define NUMBER_OF_THREADS 9   // Created a varable of how many times the process will be forked

/* This method will create multiple variables to keep track of the semaphore
 * process. Depending on the situation of the semaphore, it will either lock or unlocked
 * based  on the ID beidng odd or even. Finally it will return the  deadlock_count_number
 * of deadlocks endeadlock_count_numberered through a process.
 */

int getSemaphores(int thread_id, struct semaphores *sems) {
	int semaphore_value;//create semaphore_value
	int both_semphores = 0;//Create both_semphores
	int deadlock_count_number;//Created deadlock_count_number
	int timed_out;//create varable of timed_out
	if(thread_id % 2 == 0) {//even
		deadlock_count_number = lock_two_sems((sems->screen_semaphore), (sems->keyboard_semaphore));//call method
	}
  else {
		deadlock_count_number = lock_two_sems((sems->keyboard_semaphore), (sems->screen_semaphore));//call method
	}
	return deadlock_count_number;//return deadlock_count_number
}
/*
 * This method will have some local varables. For this thread input, threads
 * call this process to prompt the user. Determining if the user input is valid or
 * not.
 */

void *process(struct semaphores *sems) {
	int thread_id = sems->id;//set the thread_id to sems id
	printf("Executing process for THREAD_%d.\n", thread_id);//print statement
	int deadlock_count_number = 1;//intialized deadlock_count_number
	char input[80] = "w";//get the input of 80 characters
	deadlock_count_number += getSemaphores(thread_id, sems);//deadlock_count_number to get semaphores
	do {
		printf("%i Enter less than 80 characters or q for quit\n", thread_id);//ask user for input
		scanf("%s", input);//read user input
		printf("Thread_%d: You have entered %s \n", thread_id, input);//print statement
	}
  while(strcmp(input, "q") != 0);//user enters q
	if(sem_post((sems->screen_semaphore)) == -1) {
		perror("ERROR: sem_post for Screen has failed");//error message
	}
  else {
		printf("THREAD_%d: Screen has been unlocked\n", thread_id);//print message
	}
	// unlock the keyboard
	if(sem_post((sems->keyboard_semaphore)) == -1) {
		perror("ERROR: sem_post for keyboard failed");//ERROR message
	}
  deadlock_count_number += getSemaphores(thread_id, sems);//set deadlock_count_number of getSemaphores
	printf("\nThread_%d: has %d number of deadlock(s) encoutered\n",thread_id, deadlock_count_number);//print statement
	if(sem_post((sems->screen_semaphore)) == -1) {
		perror("ERROR: sem_post for screen_semaphore failed");//ERROR message
		printf("THREAD_%d: Screen Semaphore has unlocked.\n", thread_id);//print statement
	}
	if(sem_post((sems->keyboard_semaphore)) == -1) {
		perror("ERROR: sem_wait for Keyboard Semaphore has failed.");//ERROR message
	}
  else {
		printf("THREAD_%d: Keyboard Semaphore has been unlocked\n", thread_id);//print statement
	}
  sem_post(sems->counter);//unlocked counter
	printf("Thread_%d: Exiting the process.\n", thread_id);//print statement
}

/*
 * This method is the main methdod where the calling of the both methods.
 * Overall
 * the method will create a Mother process that creates the semaphore and spawns
 * the rest of the threads used for the program
 */

int main() {
  sem_t *keyboard, *screen;//created sem_t of keyboard and screen
  struct semaphores sems;//created a stuct of sems
  sem_t* counter = sem_open("/counter", O_CREAT, 0666, 0);//create sem_t of counter
	int i;//intialized varable i
	for(i = 0; i < NUMBER_OF_THREADS; i++){
		sems.id = i;//set the id to i
    if(fork() == 0){
      if((sems.keyboard_semaphore = sem_open("/keyboard4", O_CREAT, 0666, 1)) == NULL){
        perror("sem_open");//error message
        abort();//abort the process
      }
      //printf("sems.keyboard_semaphore == %p\n", sems.keyboard_semaphore);
      if((sems.screen_semaphore = sem_open("/screen4", O_CREAT, 0666, 1)) == NULL){
        perror("sem_open");//error message
        abort();//abort the Process
      }
      //printf("sems.screen_semaphore == %p\n", sems.screen_semaphore);
      if((sems.counter = sem_open("/counter", 0)) == NULL){
        perror("sem_open");//error message
        abort();//abort the process
      }
      //printf("sems.counter == %p\n", sems.counter);
      process(&sems);//sems.screen_semaphore = screen;
      printf("Process_%d: has been created\n",sems.id );
    	sem_close(sems.counter);//close sems counter
      sem_close(sems.screen_semaphore);//close sems screen_semaphore
      sem_close(sems.keyboard_semaphore);//close sems keyboard_semaphore
      exit(0);
    }
    else{
      printf("Thread_%d has been created.\n", i + 1);//print statement
    }
	}
	for(i = 0; i < NUMBER_OF_THREADS; i++){
    sem_wait(counter);
  }
	return 0;
}
