//
// Created by mohsin on 28/3/21.
//

#include  <stdio.h>
#include  <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX_CON_PROCESS 64

/* Splits the string by space and returns the array of tokens
*
*/
pid_t child_pids[64];

char **tokenize(char *line)
{ // pointer to array of pointers to token
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));

    // indivual token pointer, Temporary: freed at the end of function call
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));

    //token index is counter of char in each token
    // token number is counter of tokens
    int i, tokenIndex = 0, tokenNo = 0;

    // loop reads line one character at a time
    for(i =0; i < strlen(line); i++){
        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t'){

            token[tokenIndex] = '\0';
            if (tokenIndex != 0){ // for handling empty command entered
                tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0;
            }
        } else {
            token[tokenIndex++] = readChar;
        }
    }
    free(token);
    tokens[tokenNo] = NULL ; //setting last token ptr as NULL
    return tokens;
}

// IO redirection
void io_redirect(char **tokens){
    /* Assuming in a single command only one fd will
     * be redirected
     * @param: tokens is array of pointers to char[]
     * @param: int type 0 for input, 1 for output
     * returns:
     * io_redir_struct
     */
    int index=-1, i;
    for (i=0; tokens[i]!=NULL; i++)
    {
        if (strcmp(tokens[i],"<") == 0){
            index = i;
            int fd = open(tokens[index + 1], O_RDONLY );
            if (fd == -1)
                fprintf(stderr, "error opening file \n");
            dup2(fd, STDIN_FILENO);
            close(fd);
            break;
        } else if (strcmp(tokens[i],">") == 0){
            index = i;
            int fd = creat(tokens[index + 1], 0644);
            if (fd == -1) {
                //fprintf(stderr, "error opening file \n");
                exit(11);
            } else if ( fd >= 0 ){
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            break;
        }
    }
    if (index != -1) {
        // free tokens from redirection character onward
        for (i = index; tokens[i] != NULL; i++){
            free(tokens[i]);
            tokens[i] = NULL;
            // #?#?# if free makes the ptr null then this is not needed
        }
    }
}


int is_valid_input(int a, int b, int c){
    if ( (!a && (!b || !c)) || (!b && !c) )
        return 1;
    else
        return 0;
}


// Decide Parallel Serial Foreground or background
int check_mode(char **tokens){
    /*
     * @param char **tokens: pointer to all tokens
     * returns: 0 for forground
     *          1 for background
     *          2 for serial
     *          3 for parallel
     * Time Complexity: O(N)
     */
    int serial=0, parallel=0, background=0;
    for(int i=0;tokens[i]!=NULL;i++) {
        if (strcmp(tokens[i],"&")==0){
            if (tokens[i+1] == NULL) {
                free(tokens[i]);
                tokens[i] = NULL;
            }
                background = 1;
        }
        else if (strcmp(tokens[i], "&&")==0)
            serial = 1;
        else if (strcmp(tokens[i], "&&&")==0)
            parallel = 1;
    }
    if (!serial && !parallel && !background){
        // This means process should run in foreground
        return 0;
    }
        // check
    else if ( is_valid_input(serial, parallel, background)){
        if (background)
            return 1;
        else if (serial)
            return 2;
        else if (parallel)
            return 3;
    }
    else {
        fprintf(stderr,"Do not use multiple special characters\n");
        return -1;
    }
    return -1;
}

void insert_pid(pid_t *array, pid_t pid ){
    int i;
    for (i=0; array[i]!=NULL && i<= MAX_CON_PROCESS; i++);
    array[i] = pid;
    array[i+1]=NULL;
}


// Part A
int parser(char **tokens, int background){
    int wstatus;
    pid_t pid = fork();

    if ( pid < 0 ){
        // error occurred
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0){
        io_redirect(tokens);
        //printf("beginining child process %s ...", tokens[0]);
        sleep(100);
        //printf("beginining child process ended");
        if ( execvp(tokens[0], tokens) < 0 ){
            exit(-1);
        }else{
            exit(0);
        }
    }
    else{ //Here background execution can be passed.
        if (background == 0){
            wait(&wstatus);// Waits for one child completion
            // Ensure all children have ended <
            if (WIFEXITED(wstatus))
                return 0;
            else
                return -1;
        }else
            insert_pid(child_pids, pid);
    }
    return 0;
}

// CHDIR part B
int chdir_routine(char **tokens){
    int flag;
    int i;
    for(i=1;tokens[i]!=NULL;i++){
        // loop over tokens and reach the end of path
        flag = chdir(tokens[i]);
        if (flag == -1){
            // if error occurs go back to starting directory
            printf("No such directory exists\n");
            while (i != 1){
                chdir("..");
                i--;
            }
            break;
        }
    }
    return flag;
}


void chdir_or_child(char **tokens, char *line, char *hist_line, int background, int *exit){
    if (strcmp(tokens[0],"exit") == 0)
        *exit = 1;
    else if (strcmp(tokens[0], "cd") != 0 && strcmp(tokens[0], "!!") != 0) {
        if ( parser(tokens, background) == 0 && (strcmp(line,"!!") != 0))
            strcpy(hist_line, line);
    } else if (strcmp(tokens[0], "cd") == 0)
        chdir_routine(tokens);
}

//wrapper for parser for serial and parallel execution
void wrapper_parser(char **tokens, char *line, char *hist_line, int *exit_){
    int mode = check_mode(tokens);
    char separator[3];
    if (mode == 2){
        strcpy(separator, "&&\0");
    } else if (mode == 3){
        strcpy(separator, "&&&\0");
    }
    int background =1; // Parallel or Background
    if (mode == 0 || mode == 2){// foreground and Serial
        background = 0;
    }
    if (mode == 0 || mode == 1) {
        //foreground or background
        // will run in single parse call
        chdir_or_child(tokens, line, hist_line,background, exit_);
    }
    else if (mode == 2 || mode == 3){
        //serial or parallel
        char **temp_tokens = NULL;
        int j = 0, i= 0;
        do{
            if (temp_tokens == NULL) // Allocate memory
                temp_tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
            if (tokens[i] != NULL && strcmp(tokens[i],separator)!=0 )
                temp_tokens[j++] = tokens[i];
            else{
                //strcpy(temp_tokens[j++],"\0");
                temp_tokens[j] = NULL;
                chdir_or_child(temp_tokens, line, hist_line, background, exit_);
                j=0;
                free(temp_tokens);
                temp_tokens = NULL;
            }
            // this temp_tokens will have pointers till \n or &&
        } while(tokens[i++]!=NULL);
    }
}

void exit_routine(){
    for (int i=0;child_pids[i]!=NULL && child_pids<=MAX_CON_PROCESS;i++)
        kill(child_pids[i], SIGTERM);
}


void free_routine(char **tokens){
    int i;
    for(i=0;tokens[i]!=NULL;i++){
        free(tokens[i]);
    }
    free(tokens);
}

void swap(pid_t *array, int a, int b){
    pid_t temp;
    temp = array[a];
    array[a] = array[b];
    array[b] =temp;
}


void delete_pid_index(pid_t *array, int index ){
    int j;
    for (j=index; j < MAX_CON_PROCESS && array[j+1] == NULL; j++)
        swap(array, j,j+1);
    array[j] = NULL;
}



void reap_finished_children(pid_t *array){
    /*
     * Reaps children that have finished executing
     */
    int wstatus;
    pid_t x;
    for (int i=0; array[i] != NULL && i<=MAX_CON_PROCESS;i++){
        x = waitpid(array[i], &wstatus, WNOHANG);
        if (x == array[i])
            delete_pid_index(array, i);
    }
}


//Back Ground Execution


int main(int argc, char* argv[]) {
    char  line[MAX_INPUT_SIZE]; // For history feature make a copy of this
    char hist_line[MAX_INPUT_SIZE];
    char  **tokens;
    int exit_ = 0;
    //pid_stack.top_of_stack = 0;
    hist_line[0] = '\0';
    FILE* fp;
     //  Assuming at one time no more than 64 child process are working
    child_pids[0] = NULL;
    if(argc == 2) {
        fp = fopen(argv[1],"r");
        if(fp < 0) {
            printf("File doesn't exists.");
            return -1;
        }
        // Add logic to run the shell command via terminal
    }
    while(1) {
        /* BEGIN: TAKING INPUT */
        bzero(line, sizeof(line));
        if(argc == 2) { // batch mode
            if(fgets(line, sizeof(line), fp) == NULL) { // file reading finished
                break;
            }
            line[strlen(line) - 1] = '\0';
        } else { // interactive mode
            printf("$ ");
            scanf("%[^\n]", line);
            getchar(); // gets the enter key
        }
//        printf("Command entered: %s (remove this debug output later)\n", line);
        /* END: TAKING INPUT */

        line[strlen(line)] = '\n'; //terminate with new line
        tokens = tokenize(line);

        //do whatever you want with the commands, here we just print them
        reap_finished_children(child_pids);
        if ( tokens[0] != NULL ) {
            // History Feature START
            if (strcmp(tokens[0], "!!") == 0){
                // Second AND confirms that history is not NULL
                if (hist_line[0] != '\0'){
                    free_routine(tokens);
                    tokens = tokenize(hist_line);
                    printf("Command entered: %s\n", hist_line);
                }else if (hist_line[0] == '\0')
                    printf("No commands in history\n");
            }
            // History Feature End
            wrapper_parser(tokens, line, hist_line, &exit_);
        }
            //
//        for(i=0;tokens[i]!=NULL;i++){
//            printf("found token %s (remove this debug output later)\n", tokens[i]);
//        }
        // Freeing the allocated memory
        free_routine(tokens);
        if (exit_){
            exit_routine();
            exit(0);
        }
    }
    return 0;
}
