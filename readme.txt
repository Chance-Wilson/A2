To execute this program, navigate in a terminal to "/Wilson" and use the following command 
"./a.out <inputfile> <timeslice> where the input file is a text file and timeslice is a non-negative integer value >0. If you want to compile the files on your own before executing the program. use the command gcc <filename> on primeCheck.c and A2.c in that order. The output file of primeCheck.c must be named primeCheck for the program to work as intended.

It is also important to note that the in.txt file must only contain numerical values in the format of <processID> <burstTime>. The program will not run as intended if your input file contains column headers such as "Pid" and "Burst Time" before the numerical values. Feel free to look at the provided in.txt file and change/add values as you wish in that format for further testing.

This program fully satifies the requirements of A2 and the output of the file should demonstrate that processes are successfuly created, scheduled, suspdended, and killed upon completion of their burst time. processes which are not completed during the inital time slice are moved onto an FCFS queue for further processing.
