#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

typedef long unsigned int BigInt;
BigInt currNumber = 0;
int predefinedID = 0;
int realPID = 0;

BigInt primeNum(BigInt number){
  //finds next highest prime number reletive to 
  //the number given
while(1)
{
    bool isPrime = true;

 for(BigInt i = 2; i < number;i++) 
 {
  if ((number % 10) == 0)
  {
    //printf("multiple of 10: %lu\n", number);
  }
   if(number%i == 0 && i != number)
   {
    //the number is not prime
    number += 1;
    isPrime = false;
    break;
   }
 }
 if(isPrime)
 {
  //puts("new prime!");
  currNumber = number;
  break;
 }
}
return number;
}

void sighandler(int signum){
  if(SIGTERM == signum){
     printf("process %d: my PID is %d: I am leaving the system. The largest prime I found was %lu\n",predefinedID, realPID, currNumber);
  }else if (SIGTSTP == signum)
  {
     printf("process %d: my PID is %d: I am about to be suspended... Highest prime number I found is %lu\n",predefinedID,realPID,currNumber);
  }
  
 
}


int main(int argc, char** argv){
  realPID = getpid();
  //printf("Hello World\n");
  if(argc != 2)
  {
    puts("insufficient arguments given to child process");
    return 1;
  }
  int itemsScanned = sscanf(argv[1],"%d",&predefinedID);
  //printf("predefined id is: ",&predefinedID);
  if(itemsScanned != 1){
    puts("PID not scanned");
    return 1;
  }
 srand(time(NULL));
 BigInt number = abs(rand());
 currNumber = number;
 number += 1000000000;
 printf("process %d: my PID is %d: I just got started. I am starting with the number %lu to find the next Prime number\n",predefinedID,realPID, number);
 signal(SIGTERM,sighandler);
 signal(SIGTSTP,sighandler);
 //generate random 10 digit number
 //Generate initial Prime Number
 number = primeNum(number);
 while(1){
  //next highest prime number
  number += 1;
  number = primeNum(number);
 }
 




 return 1;   
}