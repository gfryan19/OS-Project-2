// Project 2
// Author: Greg Ryan
// Date: 2/27/18

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "lexer.h"
#include "stringlist.h"


// readline reads and returns one line of text from the file descriptor,
// stopping at the first newline or error. It can only handle lines with
// 255 characters or less. Upon error, it prints a friendly
// error message and returns whatever text has already been read.

char *readline(int fd) {
    char *s = malloc(256); // assumes line will be 255 characters or less
    int n = 0;
    while (1) {
        int r = read(fd, s + n, 1);
        if (r == 0) {
            printf("Line didn't end, but there is no more data available in file %d\n", fd);
            s[n] = '\0'; // add the terminating zero and return the string
            return s;
        } else if (r < 0) {
            printf("There was a problem reading from file %d: error number %d\n", fd, n);
            s[n] = '\0'; // add the terminating zero and return the string
            return s;
        } else if (s[n] == '\n') {
            s[n] = '\0'; // add the terminating zero and return the string
            return s;
        } else if (n == 255) {
            printf("Already read 255 characters, but line didn't end yet in file %d\n", fd);
            s[n] = '\0'; // add the terminating zero and return the string
            return s;
        } else { 
            n++;
        }
    }
}
// isFileExecutable() returns non-zero if the file exists and is executable, //
// otherwise it returns 0.
int isFileExecutable(const char *filename) {
    struct stat statinfo;
    int result = stat(filename, &statinfo);
    if (result < 0) return 0;
    if (!S_ISREG(statinfo.st_mode)) return 0;

    if (statinfo.st_uid == geteuid()) return statinfo.st_mode & S_IXUSR;
    if (statinfo.st_gid == getegid()) return statinfo.st_mode & S_IXGRP;
    return statinfo.st_mode & S_IXOTH;
}

int main(int argc, char* argv[]) {
    setbuf(stdout, NULL);
    
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    int nowSeconds = now.tv_sec;
    int nowNanoSeconds = now.tv_nsec;
    int quit = 0;

    printf("Welcome to mini shell!\n"
           "The time is %d seconds, and %d nanoseconds since midnight, Jan 1, 1970\n"
           "Type \"help\" if you are lost\n", nowSeconds, nowNanoSeconds);
    
    while (!quit) {

        printf("What next?: ");
        char *line = readline(0); // read from standard input

        char **words = stringlist_empty();//create an empty list
        words = split_words(line); //fill the list with words

        int len = stringlist_len(words);//lenght of the list

        //debug option "-x"
        //echo back what the user typed
        if (argc == 2){
            if (strcmp(argv[1], "-x") == 0){
                printf("About to execute: %s\n", line);
            }
        }

        if (strcmp(line, "how are you") == 0) {
            printf("I am fine, thanks!\n");
        }
        else if (strcmp(line, "tell me the time") == 0) {
            struct timespec now;
            clock_gettime(CLOCK_REALTIME, &now);
            int seconds = now.tv_sec;
            int nanoseconds = now.tv_nsec;
            printf("The time is %d seconds, %d nanoseconds since midnight, January 1970.\n", seconds, nanoseconds);
        }
        else if (strcmp(line, "tell me your name") == 0) {
            printf("My name is Frank.\n");
        }
        else if (strcmp(line, "tell me your id") == 0) {
            int id = getpid();
            printf("My id is process %d.\n", id);
        }
        else if (strcmp(line, "tell me your parent's id") == 0) {
            int pID = getppid();
            printf("My parent's id is process %d.\n", pID);
        }
        else if (strcmp(line, "who am i") == 0) {
            char *user = getenv("USER");
            printf("You are %s silly.\n", user);
        }
        else if (strcmp(line, "tell me your age") == 0) {
            struct timespec now;
            clock_gettime(CLOCK_REALTIME, &now);
            int ageSeconds = now.tv_sec;
            int ageNanoseconds = now.tv_nsec;
            printf("I am about %d seconds and %d nanoseconds old.\n", ageSeconds - nowSeconds, ageNanoseconds - nowNanoSeconds);
        }
        else if (strcmp(line, "help") == 0) {
                printf("Possible commands: \n"
                       "how are you\n"
                       "tell me the time\n"
                       "tell me your name\n"
                       "tell me your id\n"
                       "tell me your parent's id\n"
                       "who am i\n"
                       "tell me your age\n"
                       "say (enter text)\n"
                       "sleep 3(3 = num seconds)\n"
                       "list (enter directory name)\n"
                       "open (filename)\n"
                       "read 3(3 = result of open)\n"
                       "close 3(3 = result of open)\n"
                       "quit\n"
                       "Enter a valid shell command such as:\"/usr/bin/emacs\" or \"emacs\"");
        }
        else if (strncmp(line, "say ", 4) ==0) {
            char *say = strtok(line," ");
            while (say != NULL){
                say = strtok(NULL, " ");
                if (say == NULL){
                    //do nothing
                }else{
                    printf("%s ", say);
                }
            }
            printf("\n");
        }
        else if (strncmp(line, "sleep ", 6) == 0) {
            char *sleep = strtok(line," ");
            sleep = strtok(NULL, " ");
            
            int pause = atoi(sleep);
            printf("Going to sleep for %d seconds....\n", pause);
            usleep(pause * 1000000);
            printf("Okay, that was a nice nap.\n");
        }
        else if (strncmp(line, "list ", 5) == 0) {
            char *list = strtok(line," ");
            list = strtok(NULL, " ");
            printf("OK, here are the things in directory: %s \n", list);
            
            DIR *directory = opendir(list);
            struct dirent *sub =  readdir(directory);

            if (sub == NULL){//Error checking
                printf("Nothing was found in this directory.\n");
            }
            
            while (sub != NULL) {
               printf("%s\n", sub->d_name);
               sub =  readdir(directory);
            }
            closedir(directory);
        } 
        else if (strcmp(line, "quit") == 0) {
            quit = 1;
        }
        else if (strncmp(line, "open ", 5) == 0) {
            char *openFile = strtok(line," ");
            openFile = strtok(NULL, " ");
         
            int numAssign = open(openFile, O_RDONLY);

            if (numAssign == -1){
                printf("There was an error opening the file.\n");
            }else{
                printf("Ok, I just opened file number %d\n", numAssign);
            }
        }
        else if (strncmp(line, "read ", 5) == 0) {
            char *readFile = strtok(line," ");
            readFile = strtok(NULL, " ");

            int numRead = atoi(readFile);
            if (numRead < 0){
                printf("Sorry, there was an error reading this file.\n");
            } else {
                char *readOut = readline(numRead);
                printf("The next line in file %d is: %s\n", numRead, readOut);
                free(readOut);
            }     
        }
        else if (strncmp(line, "close ", 6) == 0) {
            char *closeFile  = strtok(line," ");
            closeFile = strtok(NULL, " ");

            int closeIt = atoi(closeFile);
            int closed = close(closeIt);
            if (closed == -1) {
                printf("There was an error closing the file.\n");
            }
            else if (closed == 0) {
                printf("You closed %d.\n", closeIt);
            }
        } else {

            int bkgrd = 0;//checks to see if a file should run in the background
            if (len > 0 && strcmp(words[len-1], "&") == 0){
                bkgrd = 1;
                char *x = NULL;
                x = stringlist_pop(&words);
                len = len - 1;
                free(x);
            }

            char *outFile = NULL;
            char *inFile = NULL;
            int out = 0;
            int in = 0;
            int app = 0;
            int valid = 0;//check for valid command
            pid_t pid;
            int status;
            char *test1 = malloc(256);
            char *test2 = malloc(256);
            char *test3 = malloc(256);

             if (len > 0){
                 outFile = strdup(words[len - 1]);
            }

            if (len >= 5){
                inFile = strdup(words[len - 3]);
            } else {
                inFile = strdup(outFile);
            }

            if (len > 2 && strcmp(words[len - 2], ">") == 0){//checks for output
                char **x = NULL;
                x = stringlist_popn(&words, 2);
                len = len - 2;
                out = 1;
                stringlist_free(&x);
            }
            else if (len > 2 && strcmp(words[len - 2], ">>") == 0){//checks for append
                char **x = NULL;
                x = stringlist_popn(&words, 2);
                len = len - 2;
                app = 1;
                stringlist_free(&x);
            }
            if (len > 2 && strcmp(words[len - 2], "<") == 0){//checks for input
                char **x = NULL;
                x = stringlist_popn(&words, 2);
                len = len - 2;
                in = 1;
                stringlist_free(&x);
            }

            if ((strchr(words[0], '/')) != 0){    //by sticking each conditon in front of the given command
                if (isFileExecutable(words[0]) != 0){
                    valid = 1;
                } else {
                    valid = 0;
                }
            } else {//checks given command against all these conditions
                strcpy(test1, "/usr/local/bin/");
                strcat(test1, words[0]);
                if (isFileExecutable(test1) != 0){
                    free(words[0]);
                    words[0] = strdup(test1);
                    valid = 1;
                } else {
                    strcpy(test2, "/bin/");
                    strcat(test2, words[0]);
                    if (isFileExecutable(test2) != 0){
                        free(words[0]);
                        words[0] = strdup(test2);
                        valid = 1;
                    } else {
                        strcpy(test3, "/usr/bin/");
                        strcat(test3, words[0]);
                        if (isFileExecutable(test3) != 0){
                            free(words[0]);
                            words[0] = strdup(test3);
                            valid = 1;
                        } else {
                            valid = 0;
                        }
                    }
                }
                    
            }
            

            free(test1);
            free(test2);
            free(test3);

            if (valid == 0){
                printf("Sorry, that command cannot be executed. Type \"help\" for a list of valid commands.\n");
            } else {
                pid = fork();
                if (pid == 0){
                    if (out == 1){
                        int oFile = open(outFile, O_WRONLY + O_CREAT + O_TRUNC, S_IRWXU);
                        close(1);
                        dup2(oFile, 1);
                        close(oFile);
                    }
                    if (app == 1){
                        int aFile = open(outFile, O_WRONLY + O_CREAT + O_APPEND, S_IRWXU);
                        close(1);
                        dup2(aFile, 1);
                        close(aFile);
                    }
                    if (in == 1){
                        int inputFile = open(inFile, O_RDONLY, S_IRWXU);
                        close(0);
                        dup2(inputFile, 0);
                        close(inputFile);
                    }
                    execv(words[0], words);
                } else {
                    if (bkgrd == 0){
                        waitpid(pid, &status, 0);
                        printf("Process %d finished with status %d.\n", pid, status);
                    } else {
                        printf("Process %d run in background.\n", pid);
                    }
                }
            }
            free(words);
            free(outFile);
            free(inFile);
        }
        free(line);
    }
    printf("Bye!\n");
    return 0;
}
