#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"
#include "interpreter.h"
#include "shellmemory.h"
#include "readyQueue.h"
#include "scheduler.h"

int parseInput(char ui[]);
ReadyQueue *global_rq = NULL;

// Start of everything
int main(int argc, char *argv[]) {
    
    printf("Shell version 1.4 created December 2024\n");
    

    char prompt = '$';          // Shell prompt
    char userInput[MAX_USER_INPUT];  // user's total input stored here
    char subInputs[MAX_USER_INPUT];   // one instruction at a time stored here
    int errorCode = 0;          // zero means no error, default

    //init user & sub input
    for (int i = 0; i < MAX_USER_INPUT; i++) {
        userInput[i] = '\0';
        subInputs[i] = '\0';
    }
    
    


    int interactive = isatty(0); // int = 1 if from terminal, 0 if from file
    //init shell memory
    mem_init();
    global_rq = rq_create();
    if (!global_rq) { 
        perror("rq_create"); return 1; 
    }
    
    while (1) {
        if (interactive) { // check if stdin is from terminal (or file)
        printf("%c ", prompt);
        }
        
        fgets(userInput, MAX_USER_INPUT - 1, stdin);
        int l = strlen(userInput);
        if (l==0){
            // We reached EOF.
            parseInput("quit");
        }
        
        
        // Accounting for one-liner mode
        for (int i = 0; userInput[i] != '\0';) {
            
            for (int j = 0; userInput[i] != ';' && userInput[i] != '\0' && userInput[i] != '\n'; i++, j++) {
                
                subInputs[j] = userInput[i]; // storing a single instruction (stops at ;)
                
            }

            i++;
            
            errorCode = parseInput(subInputs); // parsing the instruction
            if (errorCode == -1)
                exit(99);           // ignore all other errors
            memset(subInputs, 0, sizeof(subInputs));
        }
        memset(userInput, 0, sizeof(userInput));
    }

    rq_destroy(global_rq);
    return 0;
}

int wordEnding(char c) {
    // You may want to add ';' to this at some point,
    // or you may want to find a different way to implement chains.
    return c == '\0' || c == ';' || c == ' ' || c == '\n';
}

int parseInput(char inp[]) {
    char tmp[200], *words[100];
    int ix = 0, w = 0;
    int wordlen;
    int errorCode;
    for (ix = 0; inp[ix] == ' ' && ix < 1000; ix++);    // skip white spaces
    while (inp[ix] != '\n' && inp[ix] != '\0' && ix < 1000) {
       
        // extract a word
        for (wordlen = 0; !wordEnding(inp[ix]) && ix < 1000; ix++, wordlen++) {
            
            tmp[wordlen] = inp[ix];
        }
        tmp[wordlen] = '\0';
        words[w] = strdup(tmp);
        w++;
        if (inp[ix] == '\0')
            break;
        
        ix++;
    }
    
    errorCode = interpreter(words, w);
    return errorCode;
}


