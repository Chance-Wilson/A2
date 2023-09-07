#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

//it is specified in the document that we will never
//be asked to schedule >100 items, Queue size has
//been capped to 100 for this reason
#define QUEUE_SIZE 100

/*
   Author: Chance Wilson
   Assignment number: A2
   Date of Submission 8/16/23
   Name of this file A2.c
   Short description of contents:
    The purpose of this file is to implement a CPU scheduling algorithim using 
    a multilevel Queue. the first queue will contain all items including their 
    predifed id and burst time as supplied by the user in the "in.txt" file.
    The user will also specify the initial time slice they wish to use in the 
    command line arguments for this program. Processes that do not complete 
    during the initial time slice. They are then moved onto an FCFS Queue where
    they will be scheduled a second time until they are completed. This will 
    take place only after the first Queue is emptied.
*/


//This defines the format of a FileStruct object
//this is the type of object we will be interacting
//with to find the PID and burst times from in.txt
//it also stores the realPID of the child process
//that it is running
typedef struct
{
    int PID;
    int burstTime;
    int RealPID;
}FileStruct;

//This defines the structure of our Queue
typedef struct
{
 FileStruct* Qbuffer[QUEUE_SIZE];
 int Qhead;
 int Qtail;
 bool Qempty;
}Queue;
//end of Queue.h

//This function takes a Queue struct and 
//sets the initial memory of the Queue to 0
//this prevents bugs and errors that can arrise
//from bad memory contained in the Queue
void initQ(Queue* array)
{
  for(int i = 0; i <= QUEUE_SIZE; i++)
  {
   array->Qbuffer[i] = NULL;
   
  }
   array->Qhead = 0;
   array->Qtail = 0;
   array->Qempty = true;
}

//returns a boolean value stating if the Queue
//is empty or not, uses stdbool 
bool Queue_isEmpty(Queue* array)
{
 return array->Qempty;
}

//pushes a FileStruct object onto the Queue
//each FileStruct object contains its predefined pid
//as specified in in.txt, its burst time, and the actual
//pid of its associated child process
void Queue_push(Queue* array, FileStruct* pushVal)
{
 if(array->Qempty == true)
 {
    array->Qhead = 0;
    array->Qtail = 0;
    array->Qempty = false;
 }else{
   array->Qhead += 1;
 }
 if(array->Qhead >= 100)
 {
   array->Qhead = 0;
 }
 array->Qbuffer[array->Qhead] = pushVal;
}

//this pops a FileStruct item from Queue and returns 
//said item then removes it from the Queue
FileStruct* QUEUE_Pop(Queue* array)
{
  if(array->Qempty == true)
  {
    return NULL;
  }

  if(array->Qtail == array->Qhead)
   {
        array->Qempty = true;
   }
  FileStruct* popval = array->Qbuffer[array->Qtail];
  array->Qbuffer[array->Qtail] = 0;
  array->Qtail +=1;

  if(array->Qempty == true)
  {
    array->Qtail = 0;
  }
  
  return popval;
  
}//end of Queue.c

//this function is a wrapper for the "kill" command
//it was used for bugtesting but now essentially 
//acts as the "kill" command with extra steps. 
//will print an error message if kill command fails to send
void KillWrapper(int pid, int signalNum)
{
  int result = kill(pid,signalNum);
  if (result != 0)
  {
    puts("Failled to send kill command");
  }
}


//Total lines will always be <= 100 so
//array will only ever need to be 100
FileStruct totalItems[100];

int main(int argc, char** argv)
{
 Queue MainQ;
 Queue FCFSQ;
 initQ(&MainQ); 
 initQ(&FCFSQ);
 int localPID;
 int localBurstTime;
 //Zero out all data in totalItems to remove 
 //garbage memory
 memset(totalItems, 0, sizeof(totalItems));
 if (argc != 3)
 {
  //this if block determines if user provided the 
  //proper amount of arguments to execute the program
  printf("Less than 3 arguments given");
  return 1;
 }   
  char* inFileStr = argv[1];
  char* timeSliceStr = argv[2];
  int TimeArg;
  int parseCheck = sscanf(timeSliceStr, "%d",&TimeArg);
  if (parseCheck == 0)
  {
    puts("Default time slice not scanned");
    return 1;
  } 

  FILE *InStrm = fopen(inFileStr,"r");
  int currLine = 0;
  while(feof(InStrm) == 0)
  {
   int itemsScanned = fscanf(InStrm,"%d %d\n", &localPID, &localBurstTime);

   if(itemsScanned != 2)
   {
    puts("Improper file format");
    return 1;
   }
   FileStruct* currentItem = &totalItems[currLine];
   currentItem->burstTime = localBurstTime;
   currentItem->PID = localPID;

   Queue_push(&MainQ, currentItem);

   currLine += 1; 
  }

  //at this point the first Queue is populated with all 
  //File struct objects from in.txt start poping items 
  //and schedule them until the first Queue is empty 
  //then move to FCFS Queue
  while(!Queue_isEmpty(&MainQ))
  {
    FileStruct* currentItem = QUEUE_Pop(&MainQ);
    //current process to fork/execute
    char idBuffer[20];
      memset(idBuffer, 0, sizeof(idBuffer));
      sprintf(idBuffer,"%d",currentItem->PID);
    __pid_t forkResult = fork();
    if(forkResult == -1)
    {
     puts("Failed to create child process");
     return 1;
    }
    if(forkResult == 0)
    {
      
      //we have succesfully entered the child process
      execl("primeCheck","primeCheck",idBuffer,(char*) NULL); 
      exit(-1);
    }
    currentItem -> RealPID = forkResult;
    int remainingTime = 0; 
    if(currentItem->burstTime <= TimeArg)
    {
      sleep(currentItem->burstTime);
      
    }else{
      remainingTime = currentItem->burstTime - TimeArg;
      sleep(TimeArg);
      
    }
    

    currentItem->burstTime = remainingTime;

    if(remainingTime > 0)
    {
      KillWrapper(forkResult, SIGTSTP);
      Queue_push(&FCFSQ,currentItem);
    }else{
      KillWrapper(forkResult, SIGTERM);
    }
  }

  //at this point FCFSQ is populated and the mainQ 
  //is completely emptied
  while(!Queue_isEmpty(&FCFSQ))
  {
    FileStruct* currentItem = QUEUE_Pop(&FCFSQ);
    KillWrapper(currentItem->RealPID,SIGCONT);
    sleep(currentItem->burstTime);
    KillWrapper(currentItem->RealPID,SIGTERM);
  }

}
