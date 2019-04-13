# MiniShell Project
Project 2 for my Operating Systems course: msh.c

With the exception of the readLine and isFileExecutable functions provided by my professor, msh.c is all original code

This is my minishell or msh project.
The purpose of this project was to create a program that functioned like the linux shell terminal.

The following files were provided by my professor for use in this project:
lexer.h - Header file for lexer library.
lexer.c - Implementation for lexer library.
stringlist.h - Header for string list utilities.
stringlist.c - Implementation for string list utilities.
examples.txt - Some example command lines you can use for testing.
soln_output.txt - Output one should expect by running the examples.
Makefile - Used to compile and test your program.

The Assignment:
Suppose you work for a company that is in the process of creating the ULTIMATE Secure Operating System. As we all know, complexity is the enemy of security. Your company has noticed that the common command-line shells, like /bin/bash, /bin/csh, and /bin/sh are extremely complex, and so has decided to completely eliminate them in favor of a much smaller and simpler shell. (You might argue that existing shells are "tried and true" and that it is a terrible idea to try to re-implement widely used and widely understood software on a whim, but you aren't the boss. Sorry.) You are in charge of developing msh, the mini-shell program. (You wanted to name it ssh for secure shell, but alas, that's already used as the name for a very widely used remote secure shell program.) The details of what msh must do are spelled out below. But there are also some rules about what your program must not do. Specifically:

Your program must not use any C++, because C++ is complex and hard to get right. Stick to plain C as in previous projects. (Of course, C is simple and so also hard to get right!)
Your program may not rely in any way on existing shells like /bin/bash, /bin/csh, or /bin/sh. You should not exec() such programs, you should not rely upon them to do input or output redirection, etc.
Your program may not use the system() or popen() library functions, or any similar functions, since these essentially just invoke /bin/sh internally.
Thus it is your program that must handle the details of forking and exec'ing programs, redirecting input and output, and setting up pipes between processes.

Chat should start by printing a welcome message, including the current time. See below for hints on working with time. Here are the basic command chat should support:

how are you -- respond with any message you like
tell me the time -- respond with the current time (see below for hints)
tell me your name -- respond with any message you like
tell me your id -- respond with a message showing the chat process's id
tell me your parent's id -- respond with a message showing the parent process id
tell me your age -- respond with a message showing how much time elapsed since the process started
who am i -- respond with the user's login name (see below for hints)
help -- respond with a list of commands that your program understands
quit -- respond with a goodbye message then exit the program
If a request is not understood, your program should print a friendly error message, such as "Sorry, I don't know how to do that".

Add chat commands that take arguments. When the user types say [some word or phrase], chat should respond by just printing that word or phrase.

When the user types sleep [amount of time], chat should go to sleep for that amount of time.

So far, our personal assistant isn't very helpful, since it can't do much other than telling the time and sleeping. Add a command to list all the entries (files, directories, etc.) in a directory of the user's choice:

list [name of directory] -- print a list of all the entries in that directory.

Finally, add commands for opening, reading from, and closing files, as follows:
open [name of file] -- open that file and print a message indicating the number for the corresponding file descriptor (or an error message if the file can't be opened)
read [file descriptor] -- read and print one line of text from the given file descriptor, or print an error if something went wrong
close [file descriptor] -- close the given file descriptor, or print an error if something went wrong


msh must be able to execute external commands. If the user types:

    What next? /usr/bin/emacs foo.txt bar.c
then your program should fork-and-exec the program /usr/bin/emacs, passing it the specified arguments. Similarly, if the user types:

    What next? ./countdown Alice 10
then, assuming the countdown progam is in the current directory, your program should fork-and-exec the countdown program, passing in the specified arguments.

After forking and executing the external program, you should use waitpid() to wait for that new child process to exit. Once it does so, print a message indicating the child PID and it's exit status.
Before attempting to exec the external command, your program should first check to make sure the file exists and that it is executable. 
If the command requested by the user does not exist, is not executable, or if the exec() call fails for any other reason, your program should print a friendly error message saying so.

To facilitate debugging and auditing, your program should also support a "-x" option on the command line. If invoked like this:
./msh -x
Then after reading each line of input from the user, the program should echo that line back to standard output, saying e.g. "About to execute: command".


Making the user type the full path to each external command, e.g. /usr/bin/emacs is annoying, especially since most commands are stored in a few common standard directories. Change the way your program handles external commands as follows. If the program name for an external command has a '/' in it, we will assume the user typed the full path, so don't do anything special. But if the program name does not have a '/', search in the following locations, in order, to find the appropriate program: /usr/local/bin, /bin, and /usr/bin.

For example, if the user types "/usr/bin/emacs", your program should execute the external program "/usr/bin/emacs" exactly as the user typed it. But if the user types just "emacs", then the program should:

first check if "/usr/local/bin/emacs" can be executed;
if not, it should next check if "/bin/emacs" can be executed;
if not, it should finally check if "/usr/bin/emacs" can be executed.
Your program should only run the first program that it finds. If none of those directories contain the "emacs" program, print a friendly error message, as before.


Some programs, like emacs, take a long time to finish. It is nice to be able to run such programs in the background. This way the user can keep using the shell for other commands while the external command executes in the background.

If the last word of the command line is "&", then don't wait for it to finish. Instead, just print the child's PID immediately then ask the user what's next.

For example:

    What next? /usr/bin/emacs foo.c &
    Process 9719 run in background.
    What next?

Some programs need lots of standard input or produce lots of standard output, and it is nice to be able to redirect these to use files instead of the keyboard and screen. This is nice for automation and auditing as well, so we can save the results of one command in a file, then later examine it or feed it as the input to some other command.

If the second-to-last word of the command line is ">", then redirect the standard output of the child process so that it goes into whatever file is specified as the last word on the command line. For example:

    What next? /bin/ls -l > output.txt
    Process 9720 finished with status 0.
    What next?
    
Similiarly, you program should support appending output to a file. That is, if the second-to-last word of the command line is ">>", then redirect the standard output of the child process so that it appends to the end of whatever file is specified as the last word on the command line. For example:

    What next? ls -l >> output.txt
    Process 9721 finished with status 0.
    What next?
This is identical to the regular output redirection, except that if the output.txt file already exists, then it is not truncated, but instead the new output is appended to the file. 

Finally, your program should support input redirection. If the second-to-last word of the command line is "<", then redirect the standard input of the child process so that it reads from whatever file is specified as the last word on the command line. For example:

    What next? grep root < /etc/passwd
    root:x:0:0:root:/root:/bin/bash
    operator:x:11:0:operator:/root:/sbin/nologin
    Process 9722 finished with status 0.
    What next?
Here, the input file must already exist. If it does not, you should print a friendly error message and not execute the command.

Here is the full list of features you need to implement:

All commands understood by your chat program
Debug option "-x" for your main program turns on echo for each command line
External commands with arguments: /bin/ls -l
A search path for external commands: ls -l
Background processes (extra credit): myprog &
Redirecting output: myprog > myoutput
Appending redirected output: myprog >> myoutput
Redirecting input: myprog < myinput
Any combination of the above, e.g.: grep "root" | tr a-z A-Z < /etc/passwd > result.txt & (this command will not work because the pipe ( | ) function is not implemented.










