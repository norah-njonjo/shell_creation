#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "shellmemory.h"
#include "shell.h"
#include "readyQueue.h"
#include "scheduler.h"

int MAX_ARGS_SIZE = 6;

int badcommand() {
    printf("Unknown Command\n");
    return 1;
}

// For source command only
int badcommandFileDoesNotExist() {
    printf("Bad command: File not found\n");
    return 3;
}

int help();
int quit();
int set(char *var, char *value);
int print(char *var);
int source(char *script);
int exec(int nfiles, char *filenames[], char *policy);
int echo(char *string);
int my_ls();
int compare_filenames(const void *a, const void *b);
int my_mkdir(char *dirname);
int my_touch(char *filename);
int my_cd(char *dirname);
int run(char *command, char *arg);
int isAlphanum(char *input);
int isNum(char a);
int isUpp(char a);
int badcommandFileDoesNotExist();

// Interpret commands and their arguments
int interpreter(char *command_args[], int args_size) {
    int i;

    if (args_size < 1 || args_size > MAX_ARGS_SIZE) {
        return badcommand();
    }

    for (i = 0; i < args_size; i++) {   // terminate args at newlines
        command_args[i][strcspn(command_args[i], "\r\n")] = 0;
    }

    if (strcmp(command_args[0], "help") == 0) {
        //help
        if (args_size != 1)
            return badcommand();
        return help();

    } else if (strcmp(command_args[0], "quit") == 0) {
        //quit
        if (args_size != 1)
            return badcommand();
        return quit();

    } else if (strcmp(command_args[0], "set") == 0) {
        //set
        if (args_size != 3)
            return badcommand();
        return set(command_args[1], command_args[2]);

    } else if (strcmp(command_args[0], "print") == 0) {
        if (args_size != 2)
            return badcommand();
        return print(command_args[1]);

    } else if (strcmp(command_args[0], "source") == 0) {
        if (args_size != 2)
            return badcommand();
        return source(command_args[1]);

    } else if (strcmp(command_args[0], "echo") == 0) {
        if (args_size != 2)
            return badcommand();
        return echo(command_args[1]);
    } else if (strcmp(command_args[0], "my_ls") == 0) {
        if (args_size != 1) {
            printf("OOPS WRONG FORMAT\n");
            return badcommand();
        }
        return my_ls();
    } else if (strcmp(command_args[0], "my_mkdir") == 0) {
        if (args_size != 2) {
            printf("OOPS WRONG FORMAT\n");
            return badcommand();
        }
        return my_mkdir(command_args[1]);
    } else if (strcmp(command_args[0], "my_touch") == 0) {
        if (args_size != 2) {
            printf("OOPS WRONG FORMAT\n");
            return badcommand();
        }
        return my_touch(command_args[1]);
    } else if (strcmp(command_args[0], "my_cd") == 0) {
        if (args_size != 2) {
            printf("OOPS WRONG FORMAT\n");
            return badcommand();
        }
        return my_cd(command_args[1]);
    } else if (strcmp(command_args[0], "run") == 0) {
        if (args_size != 3) {
            printf("OOPS WRONG FORMAT\n");
            return badcommand();
        }
        return run(command_args[1], command_args[2]);
    } else if (strcmp(command_args[0], "exec") == 0) {
        if (args_size < 3 || args_size > 6) {
            printf("OOPS WRONG FORMAT\n");
            return badcommand();
        }

        int nfiles = args_size - 2;               // exec + N files + POLICY
        if (nfiles==1) return source(command_args[1]);
        
        char *filenames[3] = {NULL, NULL, NULL};
        for (int i = 0; i < nfiles; ++i) filenames[i] = command_args[1 + i];
        char *policy = command_args[args_size - 1]; // the scheduling policy

        return exec(nfiles, filenames, policy);
    } else {
           // printf("command_args[0] = ""%s""\n",command_args[0]);
            return badcommand();
    }
}

int help() {

    // note the literal tab characters here for alignment
    char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING			Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
source SCRIPT.TXT	Executes the file SCRIPT.TXT\n \
echo STRING / VAR			Prints back the STRING\n";
    printf("%s\n", help_string);
    return 0;
}

int quit() {
    printf("Bye!\n");
    exit(0);
}

int set(char *var, char *value) {
    // Challenge: allow setting VAR to the rest of the input line,
    // possibly including spaces.

    // Hint: Since "value" might contain multiple tokens, you'll need to loop
    // through them, concatenate each token to the buffer, and handle spacing
    // appropriately. Investigate how `strcat` works and how you can use it
    // effectively here.
    if (isAlphanum(value) && isAlphanum(var)) {
        mem_set_value(var, value);
    }
    return 0;
}


int print(char *var) {
    printf("%s\n", mem_get_value(var));
    return 0;
}

int source(char *script) {
    int errCode = 0;
    char line[MAX_USER_INPUT];

    /*
    FILE *p = fopen(script, "rt");      // the program is in a file

    if (p == NULL) {
        return badcommandFileDoesNotExist();
    }

    fgets(line, MAX_USER_INPUT - 1, p);
    while (1) {
        errCode = parseInput(line);     // which calls interpreter()
        memset(line, 0, sizeof(line));

        if (feof(p)) {
            break;
        }
        fgets(line, MAX_USER_INPUT - 1, p);
    }

    fclose(p);
    */
    int start = next_free_line;
    int code_length = mem_load_script(script);
    //printf("LOAD: %s start=%d length=%d next_free_line=%d\n", script, start, code_length, next_free_line);
    if (code_length < 0) { 
        printf("Error with the file given.\n");
        return -1;
    }
    PCB* new_pcb = create_pcb(start, code_length);
    rq_enqueue(global_rq, new_pcb,0);
    scheduler_run(global_rq, "FCFS");

    return errCode;
}

// exec command

int exec(int nfiles, char *filenames[3], char *policy){
    // validate policy
    if (!(strcmp(policy, "FCFS") == 0 ||
          strcmp(policy, "SJF")  == 0 ||
          strcmp(policy, "RR")   == 0 ||
          strcmp(policy, "AGING")== 0)) {
        fprintf(stderr, "Invalid policy: %s\n", policy);
        return -1;
    }

    // Step 2: Check for duplicate filenames
    for (int i = 0; i < nfiles; i++) {
        for (int j = i + 1; j < nfiles; j++) {
            if (strcmp(filenames[i], filenames[j]) == 0) {
                printf("Error: Duplicate filenames not allowed\n");
                return -1;
            }
        }
    }

    // Step 4: Add PCBs to reading queue
    /*
    // First, PCB 1
    if (filenames[0]) {
        int start = next_free_line;
        int code_length = mem_load_script(filenames[0]);
        //printf("LOAD: %s start=%d length=%d next_free_line=%d\n", script, start, code_length, next_free_line);
        if (code_length < 0) { 
            printf("Error with the file given.\n");
            return -1;
        }
        PCB* new_pcb1 = create_pcb(start, code_length);
        rq_enqueue(global_rq, new_pcb1);
    } 
    // PCB 2 
    if (filenames[1]) {
        int start = next_free_line;
        int code_length = mem_load_script(filenames[1]);
        //printf("LOAD: %s start=%d length=%d next_free_line=%d\n", script, start, code_length, next_free_line);
        if (code_length < 0) { 
            printf("Error with the file given.\n");
            return -1;
        }
        PCB* new_pcb2 = create_pcb(start, code_length);
        rq_enqueue(global_rq, new_pcb2);
    } 
    // PCB 3 if it exists
    if (filenames[2]) {
        int start = next_free_line;
        int code_length = mem_load_script(filenames[2]);
        //printf("LOAD: %s start=%d length=%d next_free_line=%d\n", script, start, code_length, next_free_line);
        if (code_length < 0) { 
            printf("Error with the file given.\n");
            return -1;
        }
        PCB* new_pcb3 = create_pcb(start, code_length);
        rq_enqueue(global_rq, new_pcb3);
    } */

    PCB* pcbs[3] = {NULL, NULL, NULL};
    int starts[3], lengths[3];
    int loaded_count = 0;
    
    for (int i = 0; i < nfiles; i++) {
        int start = next_free_line;
        int prog_length = mem_load_script(filenames[i]);

        
        if (prog_length < 0) {
            // Loading error - clean up what we've loaded so far
            printf("Error: Could not load file %s\n", filenames[i]);
            
            // Free any scripts already loaded
            for (int j = 0; j < loaded_count; j++) {
                mem_free_program_lines(starts[j], lengths[j]);
                free_pcb(pcbs[j]);
            }
            return -1;
        }
        // Create PCB
        PCB* pcb = create_pcb(start, prog_length);
        if (pcb == NULL) {
            printf("Error: Failed to create process\n");
            
            // Clean up
            mem_free_program_lines(start, prog_length);
            for (int j = 0; j < loaded_count; j++) {
                mem_free_program_lines(starts[j], lengths[j]);
                free_pcb(pcbs[j]);
            }
            return -1;
        }
        
        // Store for later
        pcbs[loaded_count] = pcb;
        starts[loaded_count] = start;
        lengths[loaded_count] = prog_length;
        loaded_count++;

    }

    // Sort for SJF (before enqueuing)
    if ((strcmp(policy, "SJF") == 0) || strcmp(policy, "AGING") == 0) {
        //printf("INSIDE OF SJF SORTING\n");
        qsort(pcbs, loaded_count, sizeof(PCB*), compare_job_length);
    }

    for (int i = 0; i < loaded_count; i++) {
        //printf("About to ENQUEUE\n");
        rq_enqueue(global_rq, pcbs[i],0);
    }
    
    if (strcmp(policy, "FCFS")==0) {
        scheduler_run(global_rq, "FCFS");
    } else if (strcmp(policy, "SJF")==0) {
        scheduler_run(global_rq, "SJF");
    } else if (strcmp(policy, "RR")==0) {
        scheduler_run(global_rq, "RR");
    } else if (strcmp(policy, "AGING")==0) {
        scheduler_run(global_rq, "AGING");
    } 
    
    return 0;


}

int echo(char *string) {
    if (*string == '$') {   // checking that string is a variable
        char *var;
        var = string+1;
        /*if (!isAlphanum(var)) {
            return 0;
        } */
        char *value = mem_get_value(var);   // getting value of variable
        if (strcmp(value, "Variable does not exist")==0) {  // if var does not exist
            printf("\n");
        } else {    // if var exists
            printf("%s\n", value);  // printing value of var
        }
    } else { // if string not a variable
        /*if (isAlphanum(string)) {
            printf("%s\n", string); 
        } */
        printf("%s\n", string); 
    }
   return 0;
}


int my_ls() {
    
    char *files[1000];
    for (int k=0; k<1000; k++) {
        files[k] = NULL; // initializing files array
    }
    
    DIR * dirp = opendir("."); // opening current directory
    

    struct dirent *entry;
    int i = 0;
    while((entry = readdir(dirp)) != NULL) {
        files[i] = strdup(entry->d_name);
        i++;
    }
    
    
    
    

    qsort(files,i,sizeof(char*),compare_filenames);
    int p=0;
    
    // Printing sorted files in curr dir
    while (files[p]) {
        printf("%s\n", files[p]); 
        p++;
    }
    closedir(dirp);

    return 0;
}

int compare_filenames(const void *a, const void *b) {
    const char *s1 = *(const char**)a;
    const char *s2 = *(const char**)b;
    
    // Rule 1 : numbers before letters
    if (isNum(*s1) && !isNum(*s2)) return -1;
    if (!isNum(*s1) && isNum(*s2)) return 1;

    // Rule 2 : if both letters, uppercase before lowercase
    if (isUpp(*s1) && !isUpp(*s2)) return -1;
    if (!isUpp(*s1) && isUpp(*s2)) return 1;

    //Otherwise...
    return strcmp(s1,s2);
}

int isNum(char a) {
    if (a>='0' && a<='9') {
        return 1;
    }
    return 0;
}

int isUpp(char a) {
    if (a>='A' && a<='Z') {
        return 1;
    }
    return 0;
}


int my_mkdir(char *dirname) {
    if (*dirname == '$') {   // checking that string is a variable
        char *var;
        var = dirname+1;
        if (!isAlphanum(var)) {
            return 0;
        }
        char *value = mem_get_value(var);   // getting value of variable
        if (strcmp(value, "Variable does not exist")==0) {  // if var does not exist
            printf("Bad command: my_mkdir\n");
        } else { // if var exists
            
            mkdir(value, 0777);

        }
    } else {
        if (isAlphanum(dirname)) {
            mkdir(dirname, 0777);
        }
    }
    return 0;
}

int my_touch(char *filename) {
    if (isAlphanum(filename)) {
        FILE *f = fopen(filename, "wx"); 
        // will create the file if it doesn't exist already 
        // nothing happens if it already exists

        if (f == NULL) {
            perror("fopen");
        } else {
            fclose(f);
        }
    }
    return 0;
}

int my_cd(char *dirname) {

    if (isAlphanum(dirname)) { // making sure dir is alphanum

       int result = chdir(dirname); 
        if (result !=0) { // means directory does not exist
            printf("Bad command: my_cd\n");
        }

    }
    
    return 0;
}

int run(char *command, char *arg) {
    fflush(stdout);
    pid_t pid = fork();
    if (pid==0) { // so we are in child
        char *args[3];
        args[0] = command;
        args[1] = arg;
        args[2] = NULL;
        execvp(command, args);
    } else {
        wait(NULL); // parent waits for child to finish
    }
    return 0;
}

int isAlphanum(char *input){
    char *pt = input;
    for (int i=0; i<strlen(input); i++, *pt++){
        if (!((*pt >= '0' && *pt <= '9') ||   // must be either a digit
              (*pt >= 'A' && *pt <= 'Z') ||   // or an uppercase letter
              (*pt >= 'a' && *pt <= 'z'))) {  // or a lowercase letter
            return 0; // found non-alphanumeric character
        }
    }
    return 1;
}
