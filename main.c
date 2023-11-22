/*
 * Skeleton code for Lab 2 - Shell processing
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

/*
 * Main function for shell.
 */
int
main(int argc, char *argv[])
{
	int quiet = 0, parseonly = 0;
	char input[BUFSIZ]; //??what is buffsize?
	int i = 0, r = 0;

	// Check for '-q', '-p' option:
	//    -q: be quiet -- print no prompts
	//    -p: parse only -- do not run the cmd
	// changes parseonly or quiet flags to 1 if it exists in the command line
	for (i = 1; i < argc; i++) {
		if(strcmp(argv[i], "-q") == 0) {
			quiet = 1;
		} else if(strcmp(argv[i], "-p") == 0) {
			parseonly = 1;
		}
	}

	// while there is no feof ( end of file indicator)
	// stdin is what comes into the program
	while (!feof(stdin)) {
		parsestate_t parsestate; //this is a structure in cmdparse.h that has the pointr to the position and the pointer to the last position
		command_t *cmdlist; //this is where the command data structure is stored from cmdparse.h
		// Print the prompt
		if (!quiet) {
			printf("cs5600$ ");
			fflush(stdout); //flush everything from the buffer in case it's not there
		}

		// Read a string, checking for error or EOF
		//CHECKS FOR ERRORS
		//file descriptor stdin, stdout, .. for each process there is a table that tracks all of the file descriptors and 0 is stdin, 1 is stdout, 2 is stderr
		if (fgets(input, BUFSIZ, stdin) == NULL) {
			if (ferror(stdin) && errno == EINTR) {
				cmd_line_exec(0);
				continue;
			}
			if (ferror(stdin))
				// This function prints a description of the
				// error, preceded by 'cs5600: '.
				perror("cs5600");
			break;
		}

		// initialise the command list from cmdparse.c
		parse_init(&parsestate, input); //gives an empty structue and the input arguments. Adds the current location of the first 

		cmdlist = cmd_line_parse(&parsestate, 0); //inputs the 
		if (!cmdlist) {
			printf("Syntax error\n");
			continue;
		}

		// print the command list
		if (!quiet) {
			cmd_print(cmdlist, 0);
			// why do we need to do this?
			fflush(stdout);
		}

                // this actually invokes the command list.
                // you will fill in cmd_line_exec.
		if (!parseonly && cmdlist)
			cmd_line_exec(cmdlist);
                if (cmdlist)
                        cmd_free(cmdlist);

	}

	return 0;
}
