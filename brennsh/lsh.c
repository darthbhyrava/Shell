#include<stdio.h>	//fprintf(),printf(),stderr,getchar(),perror() 
#include<string.h>	//strtok(),strcmp()
#include<stdlib.h>	//malloc(),realloc(),free(),exit(),execvp(),EXIT_SUCCESS,EXIT_FAILURE
#include<unistd.h>	//chdir(),fork(),exec(),pid_t
#include<sys/wait.h>	//waitpid()


#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

char *lsh_read_line();
char **lsh_split_line(char *line);

int lsh_launch(char **args);
void lsh_loop();
//Shell Built-ins Section
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
char *builtin_str[] = {"cd","help","exit"};
int (*builtin_func[]) (char**) = { &lsh_cd, &lsh_help, &lsh_exit};
int lsh_num_builtins() { return sizeof(builtin_str)/sizeof(char*); }

//The execute() function will launch both builtins and processes
int lsh_execute(char ** args);

int main( int argc, char **argv)
{
	//Load config files, if any.

	//Run Command Loop
	lsh_loop();

	//Perform any shutdown/cleanup
	return EXIT_SUCCESS;
}

//Command Loop
void lsh_loop()
{
	char *line;
	char **args;
	int status;

	do {
		//Print the commandprompt
		printf("> ");
		//Read the command
		line = lsh_read_line();
		//Parse the command into program and arguments
		args = lsh_split_line(line);
		//Execute the command
		status = lsh_execute(args);

		//Free up memory
		free(line);
		free(args);
		//Use status to determine loop termination
	} while(status);
}

//Function to read the command
char *lsh_read_line()
{
	int bufsize = LSH_RL_BUFSIZE;
	int position = 0;	
	char *buffer = malloc(sizeof(char)*bufsize);
	int c;

	//Check for allocation errors
	if(!buffer) {
		fprintf(stderr, "lsh:allocation error!\n");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		//read a character (and store it in 'int', not 'char', since EOF is int)
		c=getchar();
		//Check if we've reached End of File or new line (this shell cannot take multi-line commands)
		if ( c==EOF || c=='\n')
		{
			buffer[position]='\0';
			return buffer;
		} 
		else
		{
			buffer[position++]=c;
		}

		//Check if buffer size is insufficient, if yes, then allocate more memory 
		if(position>=bufsize)
		{
			bufsize+=LSH_RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if(!buffer)
			{
				fprintf(stderr, "lsh: allocation error!\n");
				exit(EXIT_FAILURE);
			}
		}
	}

	/*This function could have been implemented in a much simpler way using the getline() function in stdio.h
	  char *lsh_read_line()
	  {
		char *line = NULL;
		ssize_t bufsize = 0; //getline allocates buffer :P
		getline(&line, &bufsize, stdin);
		return line;
	  }
	 */
}

/*Function to split the command into components
Not allowing quoting or backslash esaping*/
char **lsh_split_line(char *line)
{
	int bufsize = LSH_TOK_BUFSIZE, position=0;
	//Dynamically initiating an array of pointers instead of an array of characters like in read_line()
	char **tokens = malloc(sizeof(char*)*bufsize);
	char *token;

	//Allocation error
	if(!tokens)
	{
		fprintf(stderr, "lsh: allocation error!\n");
		exit(EXIT_FAILURE);
	}

	/*strtok() tokenizes the string; it returns pointers to within the string to the first token, and places a \0 at the end of each token.
	we store each pointer in an array of pointers*/ 
	token = strtok(line, LSH_TOK_DELIM);
	while(token!=NULL)
	{
		//Pointers to tokens being stored in array
		tokens[position]=token;
		position++;

		if(position >= bufsize) 
		{
			bufsize+=LSH_TOK_BUFSIZE;
			tokens=realloc(tokens, sizeof(char*)*bufsize);
			if(!tokens)
			{
				fprintf(stderr, "lsh: allocation error!\n");
				exit(EXIT_FAILURE);
			}
		}
		//What is happening here? Why NULL string?
		token=strtok(NULL, LSH_TOK_DELIM);
	}
	tokens[position]=NULL;
	return tokens;
}

/*lsh_launch exists instead of lsh_execute() because not all commands are programs, and some are inbuilt in shell itself
Some commands need the shell itself to undergo a change (like cd ..). For programs which do not need parent's processes changed, we have lsh_launch*/
int lsh_launch(char **args)
{
	//ProcessID datatype variables
	pid_t pid, wpid;
	int status;

	pid = fork();
	if(pid==0)
	{
		/*Child Process
		execvp is a variant of exec() which expects a program name and a vector(hence the v) of string arguments(the first argument being program name)
		the 'p' indicates that instead of the full path of the program, we will give name, and OS has to search for the name in the path
		exec() variants should never return any values, but if they do (here -1), there is an error, and perror() prints it along with program name
		 */
		if (execvp(args[0],args)==-1)
		{
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	}
	else if (pid<0) 
	{
		//Error Forking	
		perror("lsh");
	}
	else 
	{
		/*Parent Process
		waitpid() waits for process's state to change
		not all process changes mean that the process has ended
		Here, we use waitpid()'s macros to wait until process has either exited or has been killed
		*/
		do {
			wpid=waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	//Once the wait is over, we return 1 to the calling funtion to prompt for input again
	return 1;
}

//Implementation of the buitlins declared:

//The "cd" command
int lsh_cd(char **args)
{
	//If there is no second argument, then return error
	if(args[1]==NULL) 
	{
		fprintf(stderr, "lsh: expected argument to \"cd\"\n");
	} else 
	{
		//The cd command is chdir(arguments)
		if(chdir(args[1])!=0)
			perror("lsh");
	}
	//Return 1 to prompt for input
	return 1;
}

//The "help" command
int lsh_help(char **args)
{
	int i;
	printf("darthbhyrava's LSH\n");
	printf("Type in program names and their arguments, and then hit the return key.\n");
	printf("The following are built-in:\n");
	
	//Addition of extra builtins becomes easier: just add them to declarations above, and add an implementation function here.
	for(i=0;i<lsh_num_builtins();i++)
		printf("%s\n",builtin_str[i]);

	printf("Use the man command for information on other programs.\n");
	return 1;
}

int lsh_exit(char **args)
{
	//This'll signal the command loop to terminate
	return 0;
}


//The final execute() function
int lsh_execute(char **args)
{
	int i;
	if(args[0]==NULL)
	{
		//An empty command
		return 1;
	}
	
	//The builtins
	for(i=0; i<lsh_num_builtins();i++)
	{
		if(strcmp(args[0],builtin_str[i])==0)
			return (*builtin_func[i])(args);
	}
	
	//Other programs
	return lsh_launch(args);
}
