/*
 * Skeleton code for Lab 2 - Shell processing
 * This file contains skeleton code for executing parsed commands.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "cmdparse.h"
#include "cmdrun.h"
#include <stdint.h>


/* cmd_exec(cmd, pass_pipefd)
 *
 *   Execute the single command specified in the 'cmd' command structure.
 *
 *   The 'pass_pipefd' argument is used for pipes.
 *
 *     On input, '*pass_pipefd' is the file descriptor that the
 *     current command should use to read the output of the previous
 *     command. That is, it's the "read end" of the previous
 *     pipe, if there was a previous pipe; if there was not, then
 *     *pass_pipefd will equal STDIN_FILENO.
 *
 *     On output, cmd_exec should set '*pass_pipefd' to the file descriptor
 *     used for reading from THIS command's pipe (so that the next command
 *     can use it). If this command didn't have a pipe -- that is,
 *     if cmd->controlop != PIPE -- then this function should set
 *     '*pass_pipefd = STDIN_FILENO'.
 *
 *   Returns the process ID of the forked child, or < 0 if some system call
 *   fails.
 *
 *   Besides handling normal commands, redirection, and pipes, you must also
 *   handle three internal commands: "cd", "exit", and "our_pwd".
 *   (Why must "cd", "exit", and "our_pwd" (a version of "pwd") be implemented
 *   by the shell, versus simply exec()ing to handle them?)
 *
 *   Note that these special commands still have a status!
 *   For example, "cd DIR" should return status 0 if we successfully change
 *   to the DIR directory, and status 1 otherwise.
 *   Thus, "cd /tmp && echo /tmp exists" should print "/tmp exists" to stdout
 *      if and only if the /tmp directory exists.
 *   Not only this, but redirections should work too!
 *   For example, "cd /tmp > foo" should create an empty file named 'foo';
 *   and "cd /tmp 2> foo" should print any error messages to 'foo'.
 *
 *   Some specifications:
 *
 *       --subshells:
 *         the exit status should be either 0 (if the last
 *         command would have returned 0) or 5 (if the last command
 *         would have returned something non-zero). This is not the
 *         behavior of bash.
 *
 *       --cd:
 *
 *          this builtin takes exactly one argument besides itself (this
 *          is also not bash's behavior). if it is given fewer
 *          ("cd") or more ("cd foo bar"), that is a syntax error.  Any
 *          error (syntax or system call) should result in a non-zero
 *          exit status. Here is the specification for output:
 *
 *                ----If there is a syntax error, your shell should
 *                display the following message verbatim:
 *                   "cd: Syntax error! Wrong number of arguments!"
 *
 *                ----If there is a system call error, your shell should
 *                invoke perror("cd")
 *
 *       --our_pwd:
 *
 *          This stands for "our pwd", which prints the working
 *          directory to stdout, and has exit status 0 if successful and
 *          non-zero otherwise. this builtin takes no arguments besides
 *          itself. Handle errors in analogy with cd. Here, the syntax
 *          error message should be:
 *
 *              "pwd: Syntax error! Wrong number of arguments!"
 *
 *       --exit:
 *
 *          As noted in the lab, exit can take 0 or 1 arguments. If it
 *          is given zero arguments (besides itself), then the shell
 *          exits with command status 0. If it is given one numerical
 *          argument, the shell exits with that numerical argument. If it
 *          is given one non-numerical argument, do something sensible.
 *          If it is given more than one argument, print an error message,
 *          and do not exit.
 * 
 * //exit in the child
 * //exit in the parent
 *
 *
 *   Implementation hints are given in the function body.
 */

// Fork the child and execute the command in that child.
	// You will handle all redirections by manipulating file descriptors.
	//
	// This section is fairly long.  It is probably best to implement this
	// part in stages, checking it after each step.  For example:
        //
	//   --First implement just the fork and the execute in the
	//   child (as we saw in class).  This should allow you to
	//   execute simple commands like 'ls'. Some of the tests in
	//   "make test" ought to pass.
	//   --Next, add support for redirections: as a simple test,
	//   'ls > foo' followed by 'cat < foo' should display the
	//   directory listing. A few more of the tests in "make test"
	//   should pass.
	//   --Next, support parentheses (a few more tests should pass,
	//     etc.)
	//   --Then pipes (if you use our pseudocode from class as
	//   inspiration, NOTE that the logic is a bit different in this lab)
	//   --And finally the internal commands 'cd', 'exit', and
	//   'our_pwd'.
	//
	//  Throughout, you will be using system calls: fork(), dup2(),
	//  close(), open(), execvp() or execve(), exit(), chdir(), etc.
	//
	//  You can also use perror() to handle error conditions
	//  concisely (type "$ man 3 perror" in a Unix terminal to get
	//  documentation).
	//
	//  You will need to do real C programming here.
	//  Tools like gdb (the debugger) may be useful.
	//
	//  You will also find functions like strcmp() and strtol()
	//  useful. Type "man strcmp", etc. for more information.
	//
	// In the child, you should:
	//    1. Set up stdout to point to this command's pipe, if necessary;
	//       close some file descriptors, if necessary (which ones?)
	//    2. Set up stdin to point to the PREVIOUS command's pipe (that
	//       is, *pass_pipefd), if appropriate; close a file
	//       descriptor (which one?)
	//    3. Set up redirections. Use the open() system call.
	//       Hints:
	//          --For input redirections, the oflag should be O_RDONLY.
	//          --For output redirections (stdout and stderr), what
	//          oflags do you want?
	//          --Set the mode argument of open() to be 0666
	//    4. Execute the command.
	//       There are some special cases:
	//       a. Parentheses.  Execute cmd->subshell. How? Also,
	//          you won't be invoking exec() in this special case,
	//          so think carefully about how this path "ends". (What
	//          would happen if we were to return from the current
	//          function?)
	//       b. A null command (no subshell, no arguments).
	//          Exit with status 0.
	//       c. "exit".
	//       d. "cd".
	//       e. "our_pwd".
	//       f. Remember to make your implementation
	//       consistent with the specification above!
	//
	// In the parent, you should:
	//    1. Close some file descriptors.  Hint: Consider the write end
	//       of this command's pipe, and one other fd as well.
	//    2. Handle the special "exit", "cd", and "our_pwd" commands.
	//    3. Set *pass_pipefd as appropriate.
	//
	// "cd","exit","our_pwd" Hints:
	//    Recall from the comments earlier that you need to return a status,
	//    and do redirections. How can you do these things for a
	//    command executed in the parent shell?
	//    Answer: It's easest if you fork a child ANYWAY!
        //    You should divide functionality between the parent and the child.
        //    Some functions will be executed in each process. For example,
        //         --in the case of "exit", both parent and child
        //         need to exit, but only the parent's exit code "matters",
        //         --in the case of "cd", both parent and child should
        //         try to change directory (use chdir()), but only the child
        //         should error messages
        //         --in the case of "our_pwd", see "man getcwd"
	//
	//    For the "cd" command, you should change directories AFTER
	//    the fork(), not before it.  Why?
	//    Design some tests with 'bash' that will tell you the answer.
	//    For example, try "cd /tmp ; cd $HOME > foo".  In which directory
	//    does foo appear, /tmp or $HOME?  If you chdir() BEFORE the fork,
	//    in which directory would foo appear, /tmp or $HOME?
	//
static pid_t
cmd_exec(command_t *cmd, int *pass_pipefd)
{
	if (cmd == NULL){
		return -2;
	}

    (void)pass_pipefd;      // get rid of unused warning
	pid_t pid = -1;		// process ID for child
	int pipefd[2];		// file descriptors for this process's pipe
	//int pipefd[0] = STDIN_FILENO;  // read end of last pipe

	// Create a pipe, if this command is the left-hand side of a pipe.
	// Return -1 if the pipe fails.
	if (cmd->controlop == CMD_PIPE) {
		if (pipe(pipefd) == -1){printf("An error occured opening the pipe\n");exit(1);return -1;} //created pipe
	}
	
	if ((pid=fork()) == 0){
		//printf("child: ENTER CHILD %s %s\n\n", cmd -> argv[0], cmd -> argv[1]);
		if (cmd -> controlop == CMD_PIPE) { //if there is a pipe, this is what will be outputted from the child
			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO); //whatever is supposed to be outputted will go to pipefd[1]
			// STDOUT redirected to write-end
			close(pipefd[1]); //close write end
		}

		
			//printf("child: RHS %s %s\n\n", cmd -> argv[0], cmd -> argv[1]);
			if (*pass_pipefd != STDIN_FILENO){
				//pass_pipefd = pipefd[0];
				dup2(*pass_pipefd, STDIN_FILENO);
				//*pass_pipefd = pipefd[0]; //pass+pipe is the read-end of the previous pipe
				close(pipefd[0]);
				close(pipefd[1]); //close write end
				//close(pipefd[0]);
				//pass_pipefd = STDIN_FILENO;
				//redirects pipe to the current process
				//just do command?
				//printf("RHS child CMD: %s %s", cmd->argv[0], cmd->argv[1]);
				//we are now on the right side of the command
				//printf("child: we are now on the right side of the command\n");

			}
		
		if (cmd -> redirect_filename[0] != 0){
			int file = open(cmd -> redirect_filename[0], O_RDONLY, 0666);
			if (file == -1) {
				//printf("error code to open file\n"); 
				exit(1); 
				return -2;}
			dup2(file, STDIN_FILENO); //whatever was supposed to be inputted as a command will now go to stdinput
			if (cmd -> subshell != 0){
				cmd_line_exec(cmd->subshell);
				exit(0);
			}
			close(file);
		}
		if (cmd -> redirect_filename[1] != 0){ //this is where it goes first
			int file = open(cmd -> redirect_filename[1], O_CREAT | O_TRUNC | O_WRONLY, 0666);
			if (file == -1) {
				//printf("error code to open file\n"); 
				exit(1); 
				return 2;}
			dup2(file, STDOUT_FILENO); //whatever was supposed ot be outputed will now go to file2
			if (cmd -> subshell != 0){
				cmd_line_exec(cmd->subshell);
				exit(0);
			}
			close(file);
		}
		if (cmd -> redirect_filename[2] != 0){
			int file = open(cmd -> redirect_filename[0], O_CREAT | O_TRUNC | O_WRONLY, 0666);
			if (file == -1) {
				//printf("error code to open file\n"); 
				exit(1); 
				return -2;}
			dup2(file, STDERR_FILENO); //whatever was supposed to be outputed will now go to file2
			if (cmd -> subshell != 0){
				cmd_line_exec(cmd->subshell);
				exit(0);
			}
			close(file);
		}
		
		if (cmd -> subshell != 0){
			int err = cmd_line_exec(cmd->subshell);
			if (err == 1){
				//printf("error in program to execute\n"); //fyi if there a redirection arrow but the program doesn;t work, it will print the output 
				exit(1);
				return 5;} 
			exit(0); //try without this
		}
		if (cmd -> argv[0] != 0){
			//printf("parent with cmd %s\n", cmd->argv[0] );

			//pwd
			if (strcmp(cmd->argv[0], "our_pwd") == 0){
				char cwd[256];
				if (getcwd(cwd, sizeof(cwd)) == NULL){
					perror("pwd");
					exit(1);
				};
				if (cmd -> argv[1] != 0){
					printf("pwd: Syntax error! Wrong number of arguments!\n");
					exit(1);
					return -1;
				}
				
				printf("%s\n", cwd);
				exit(0);
			}

			if (strcmp(cmd->argv[0], "exit") == 0){
				exit(1);
			}

			//cd 
			if (strcmp(cmd->argv[0], "cd") == 0){
				if (chdir(cmd->argv[1]) == -1){
					perror("cd");
					exit(1);
				}
				//second argument doesn't exist
				if (cmd -> argv[1] == 0){
					printf("cd: Syntax error! Wrong number of arguments!\n");
					exit(1);
					return -1;
				}
				//third argument exists
				if (cmd -> argv[2] != 0){
					printf("cd: Syntax error! Wrong number of arguments!\n");
					exit(1);
					return -1;
				}
				
				exit(0);
			}
			
			int err = execvp(cmd->argv[0],cmd->argv);
			if (err == -1){
				printf("error in program to execute\n"); //fyi if there a redirection arrow but the program doesn;t work, it will print the output 
				exit(1);
				return -2;} 
		}
		//exit(0);
	}

	//parent process
	else if (pid > 0){
		if (cmd -> argv[0] != 0){
			if (strcmp(cmd->argv[0], "exit") == 0){
					if (cmd->argv[2] != 0){
						printf("Syntax error: There are too many arguments\n");
						return -1;
					}
					exit((intptr_t)cmd -> argv[1]);
				}
			if (strcmp(cmd->argv[0], "cd") == 0){
					chdir(cmd->argv[1]);
			}
		}

		//LHS of pipe
		if (cmd -> controlop == CMD_PIPE){
			
			dup2(pipefd[0], STDIN_FILENO);
			*pass_pipefd = pipefd[0]; //pass+pipe is the read-end of the previous pipe
			close(pipefd[0]);
			close(pipefd[1]); //close write end
			//wait(NULL);
			// send input to 
			//printf("parent: LHS INSIDE PIPE\n\n");
			// close(pipefd[1]); //close write end
			// dup2(pipefd[0], STDIN_FILENO);
			// pass_pipefd = &pipefd[0]; //pass+pipe is the read-end of the previous pipe
			// close(pipefd[0]);
		}
		//RHS of pipe and all other commands without pipe
		else{
			//printf("parent: NO PIPE?? \n\n");
			*pass_pipefd = STDIN_FILENO;
// 			On output, cmd_exec should set '*pass_pipefd' to the file descriptor
//  *     used for reading from THIS command's pipe (so that the next command
//  *     can use it). 
		}
	}

	else {
		perror("failed to fork");
	}
	return pid;
}


/* cmd_line_exec(cmdlist)
 *
 *   Execute the command list.
 *
 *   Execute each individual command with 'cmd_exec'.
 *   String commands together depending on the 'cmdlist->controlop' operators.
 *   Returns the exit status of the entire command list, which equals the
 *   exit status of the last completed command.
 *
 *   The operators have the following behavior:
 *
 *      CMD_END, CMD_SEMICOLON
 *                        Wait for command to exit.  Proceed to next command
 *                        regardless of status.
 *      CMD_AND           Wait for command to exit.  Proceed to next command
 *                        only if this command exited with status 0.  Otherwise
 *                        exit the whole command line.
 *      CMD_OR            Wait for command to exit.  Proceed to next command
 *                        only if this command exited with status != 0.
 *                        Otherwise exit the whole command line.
 *      CMD_BACKGROUND, CMD_PIPE
 *                        Do not wait for this command to exit.  Pretend it
 *                        had status 0, for the purpose of returning a value
 *                        from cmd_line_exec.
 */
int
cmd_line_exec(command_t *cmdlist)
{
	int cmd_status = 0;	    // status of last command executed
	int pipefd = STDIN_FILENO;


	while (cmdlist) {
		int wp_status;

		if (cmdlist -> controlop == CMD_BACKGROUND){
			wp_status = cmd_exec(cmdlist, &pipefd);
			cmd_status = WEXITSTATUS(wp_status);
			cmdlist = cmdlist->next;
			continue;
		}
		//printf("**wpstatus %d and cmd_status %d\n", WEXITSTATUS(wp_status), cmd_status);
		//printf("cmd_line_exec: %s %s\n\n", cmdlist -> argv[0], cmdlist -> argv[1]);
		if ((waitpid(cmd_exec(cmdlist, &pipefd), &wp_status, WUNTRACED)) == -1){
			perror("wait() error");
		}; 
		

		if ((cmdlist -> controlop == CMD_OR) && WEXITSTATUS(wp_status) == 0){
			//printf("OR Exit staatus of cmd %s = %d\n", cmdlist->argv[0], WEXITSTATUS(wp_status));
			return wp_status;
		}
		if ((cmdlist -> controlop == CMD_AND) && WEXITSTATUS(wp_status) == 1){
			//printf("AND Exit staatus of cmd %s = %d\n", cmdlist->argv[0], WEXITSTATUS(wp_status));
			return wp_status;
		}

		// If an error occurs in cmd_exec, feel free to abort().
		if (wp_status < -1){
			abort();
		}
		cmd_status = WEXITSTATUS(wp_status);
		//printf("wy isnt it changing wpstatus %d and cmd_status %d\n", WEXITSTATUS(wp_status), cmd_status);
		cmdlist = cmdlist->next;
	}

        while (waitpid(0, 0, WNOHANG) > 0);

done:
	return cmd_status;
}
