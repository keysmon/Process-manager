//Hang Ruan
//V00923058


#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

//node struct for linkedlist
struct node {
    int pid;
    char* job;
    struct node *next;
};
//declare function
void pprintList();
void pinsert(int pid, char* job);
void pdelete(int pid);
void check_zombieProcess();
void bg_entry(char* argv);
void bglist_entry();
void pstat_entry(char* pid);
void bgsig_entry(char* pid, char* cmd_type);
void usage_pman(char* argv);

struct node* headPnode = NULL;
int main(){
    
    char* CMD_BG = "bg";
    char* CMD_BGLIST = "bglist";
    char* CMD_BGKILL = "bgkill";
    char* CMD_BGSTOP = "bgstop";
    char* CMD_BGCONT = "bgstart";
	while(1){
        //readline and parse
		char* cmd = readline("PMan: > ");
        char* token = strtok(cmd," ");
        char* cmd_type = token;
        token = strtok(NULL," ");
        char* argv = token;
        
        //excuting different function depending on user input
		if (strcmp(CMD_BG, cmd_type) == 0 ){ //check bg + foo
			bg_entry(argv);
		}
		else if(strcmp(CMD_BGLIST,cmd_type)==0 && argv == NULL){ //check for bglist
			bglist_entry();
		}
		else if(strcmp(cmd_type, CMD_BGKILL)== 0 || strcmp(cmd_type, CMD_BGSTOP)==0 || strcmp(cmd_type,CMD_BGCONT)==0){
			bgsig_entry(argv, cmd_type);
		}
		else if(strcmp(cmd_type,"pstat") == 0){
			pstat_entry(argv);
		}
		else {
			usage_pman(argv);
		}
		check_zombieProcess();
	}
	return 0;
}



// bg_entry that create the process and add it to the Linkedlist
void bg_entry(char* argv){
	pid_t pid;
	pid = fork();
	if(pid == 0){
        char *argv_execvp[]= {argv, NULL};
		if(execvp(argv,argv_execvp) < 0){
			perror("Error on execvp");
		}
		exit(EXIT_SUCCESS);
	}
	else if(pid > 0) {
        // store information of the background child process in your data structures
        pinsert(pid, argv);
        return;
	}
	else {
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
}



void bglist_entry(){
    pprintList(headPnode);
    return;
}

void bgsig_entry(char* thepid, char* cmd_type){
    pid_t pid;
    pid = fork();
    char *argv[4];
    argv[0] = "/bin/kill";
    if(pid==0){
        // find out which signal sent by user
        if(strcmp(cmd_type, "bgkill") == 0){
            argv[1] = "-9";
        }else if (strcmp(cmd_type, "bgstart") == 0){
            argv[1] = "-18";
        }else{
            argv[1] = "-19";
        }
        argv[2] = thepid;
        argv[3] = NULL;
        // run kill sig command
        execvp(argv[0], argv);
        // if command fail
        printf("Killsig command failed\n");
        exit(EXIT_SUCCESS);
    }
    else if(pid > 0){
        waitpid(pid,NULL,0);
        //notify user that process has been killed
        if(strcmp(cmd_type, "bgkill") == 0){
            printf("%s has been terminated!\n",thepid);
            pdelete(atoi(thepid));
        }
        return;
    }else{
        perror("bgsig failed");
        exit(0);
    }
}









void pstat_entry(char* pid){
    pid_t ppid;
    ppid = fork();
    if(ppid == 0){
        //get (comm),state,utime,stime,rss from /proc/pid/stat
        int thepid = atoi(pid);
        char filename[100];
        sprintf(filename, "/proc/%d/stat", thepid);
        FILE *f = fopen(filename, "r");
        if (f ==NULL){
            printf("Error:   Process %d does not exist.\n",thepid);
            return;
        }
        int unused;
        char comm[20];
        char state;
        int ppid;
        int pgroup;
        int session;
        int tty_nr;
        int tpgid;
        unsigned int flags;
        unsigned long int minflt;
        unsigned long int cminflt;
        unsigned long int mafflt;
        unsigned long int cmajflt;
        float utime;//14
        float stime;//15
        long int cutime;
        long int cstime;
        long int priority;
        long int nice;
        long int num_threads;
        long int itrealvalue;
        unsigned long long int starttime;
        unsigned long int vsize;
        long int rss;
        //get voluntary_ctxt_switches and nonvoluntary_ctxt_switches from /proc/pid/stat
        int vol;
        int nonvol;
        char filename2[100];
        sprintf(filename2, "/proc/%d/status", thepid);
        FILE *f2 = fopen(filename2, "r");
        
        // get all the stat from /proc/
        fscanf(f, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %f %f %ld %ld %ld %ld %ld %ld %llu, %lu %ld" , &unused,comm, &state,&ppid,&pgroup,&session,&tty_nr,&tpgid,&flags,&minflt,&cminflt,&mafflt,&cmajflt,&utime,&stime, &cutime,&cstime,&priority,&nice,&num_threads,&itrealvalue,&starttime,&vsize,&rss);
        //printing out the stats that assignment askes
        printf("comm = %s\n", comm);
        printf("state = %c\n", state);
        printf("utime is %f\n",utime);
        printf("stime is %f\n",stime);
        printf("rss is %ld\n",rss);
        
        fscanf(f2, "voluntary_ctxt_switches:%d",&vol);
        fscanf(f2, "nonvoluntary_ctxt_switches:%d", &nonvol);
        printf("voluntary_ctxt_switches:%d\n",vol);
        printf("nonvoluntary_ctxt_switches:%d\n", nonvol);
        
        fclose(f);
        fclose(f2);
        exit(1);
    }else if(ppid > 0){
        return;
    }else{
        perror("bgsig failed");
        exit(EXIT_FAILURE);
    
    }
}

//given in sample code
void check_zombieProcess(){
	int status;
	int retVal = 0;
	while(1) {
		usleep(1000);
		if(headPnode == NULL){
			return ;
		}
		retVal = waitpid(-1, &status, WNOHANG);
       
		if(retVal > 0) {
		}
		else if(retVal == 0){
			break;
		}
		else{
			perror("waitpid failed");
			exit(EXIT_FAILURE);
		}
	}
     
	return ;
}

void usage_pman(char* argv){
    printf("Invalid Command!\n");
    return;
}


//function that print out the processes list when user call bglist
void pprintList(){
    int count = 0;
    struct node* temp = headPnode;
    while(temp!= NULL){
        printf("%d: /home/user/%s\n",temp->pid,temp->job);
        temp = temp->next;
        count++;
    }
    printf("Total background jobs:  %d\n", count);
    return;
}


// insert the process into the linkedlist structure
void pinsert(int pid, char *job ){
    //allocate space for the new node
    struct node *temp = (struct node*)malloc(sizeof(struct node));
    struct node *ptr = headPnode;
    temp->pid = pid;
    temp->job = job;
    temp->next = NULL;
    //test whether head is null
    if (headPnode == NULL){
        headPnode = temp;
    }else{
        while (ptr->next!= NULL){
            ptr = ptr->next;
        }
        ptr->next = temp;
    }
    return;
}
     

//delete a process from the linkedlist structure
void pdelete(int pid){
    struct node *ptr = headPnode;
    struct node *prev = headPnode;
            // if head is null,return
    if(headPnode == NULL){
        return;
        // if list has only head, head turns to null
    }else if(headPnode->next == NULL){
        headPnode = NULL;
        
    }else{
        if(ptr->pid == pid){
            headPnode = headPnode->next;
            return;
        }
        ptr = headPnode->next;
        while(ptr->pid != pid){
            if(ptr->next == NULL){ //if pid not found, return
                printf("pdelete: Pid not found!");
                return ;
            }else{
                prev = ptr;
                ptr = ptr->next;
            }
        }
        if(ptr->next!= NULL){
            prev->next = ptr->next;
        }else{
            prev->next = NULL;
        }
    }
    return;
}



