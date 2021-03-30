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

/* Splits the string by space and returns the array of tokens
*
*/
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
            tokens[i] = NULL;// #?#?# if free makes the ptr null then this is not needed
        }
    }
}

// Implemented Simple Shell commands using fork and exec
int parser(char **tokens){
    int wstatus;

    pid_t pid = fork();
    if ( pid < 0 ){
        //error occured
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0){
        io_redirect(tokens);
        if ( execvp(tokens[0], tokens) < 0 ){
            exit(-1); // This is done to ensure child is cancelled in case of error
        }else{
            exit(0);
        }
    }
    else{
        wait(&wstatus);// Waits for one child completion
    }
    if (WIFEXITED(wstatus)){
        return 0;
    } else{
        return -1;
    }

}

// Implementation of cd function using chdir system call
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


void free_routine(char **tokens){
    int i;
    for(i=0;tokens[i]!=NULL;i++){
        free(tokens[i]);
    }
    free(tokens);
}

int main(int argc, char* argv[]) {
    char  line[MAX_INPUT_SIZE]; // For history feature make a copy of this
    char hist_line[MAX_INPUT_SIZE];
    char  **tokens;
    int i;
    hist_line[0] = '\0';
    FILE* fp;
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

        if ( tokens[0] != NULL ) {
            // History Feature
            if (strcmp(tokens[0], "!!") == 0){
                // Second AND confirms that history is not NULL
                if (hist_line[0] != '\0'){
                    free_routine(tokens);
                    tokens = tokenize(hist_line);
                    printf("Command entered: %s\n", hist_line);
                }else if (hist_line[0] == '\0')
                    printf("No commands in history\n");
            }
            if (strcmp(tokens[0], "cd") != 0 && strcmp(tokens[0], "!!") != 0) {
                if (parser(tokens) == 0 && (strcmp(line,"!!") != 0))
                    strcpy(hist_line, line);
            } else if (strcmp(tokens[0], "cd") == 0)
                chdir_routine(tokens);
        }

//        for(i=0;tokens[i]!=NULL;i++){
//            printf("found token %s (remove this debug output later)\n", tokens[i]);
//        }
        // Freeing the allocated memory
        free_routine(tokens);

    }
    return 0;
}
