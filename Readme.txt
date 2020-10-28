Hang Ruan 00923058

To run the pman.c: run make -> then run ./file

for testing, i ran inf.c. however i found that in order for it to work, the executable file has to compiled into ./file ahead of time which means file name has to be ./file or ./inf in my case.

to create a process for example foo, user should type bg ./foo 

Makefile given in the assignment folder is used here. Linkedlist structure is used to store process data. Pman.c was built based on A1_samplecode.

for utime and stime, I used to float instead of int as specified in P1 Q&A.
