#include<stdio.h>

//Function to read the command
#define *LSH_RL_BUFSIZE 1024
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

	//This function could have been implemented in a much simpler way using the getline() function in stdio.h
	//char *lsh_read_line()
	//{
	//	char *line = NULL;
	//	ssize_t bufsize = 0; //getline allocates buffer :P
	//	getline(&line, &bufsize, stdin);
	//	return line;
	//}
}

//Function to split the command into components
//Not allowing quoting or backslash esaping
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
char **lsh_split_line(char *line)
{
	int bufsize = LSH_TOK_BUFSIZE, position=0;
	//Dynamically initiating an array of pointers instead of an array of characters like in read_line()
	char **tokens = malloc(sizeof(char*)*bufsize);
	char *token;

	//Allocation error
	if(!tokens)
	{
		frpintf(stderr, "lsh: allocation error!\n");
		exit(EXIT_FAILURE);
	}
	
	//strtok() tokenizes the string; it returns pointers to within the string to the first token, and places a \0 at the end of each token.
	//we store each pointer in an array of pointers 
	token = strtok(line, LSH_TOK_DELIM);
	while(token!==NULL)
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



int main( int argc, char **argv)
{
	//Load config files, if any.
	
	//Run Command Loop
	lsh_loop();

	//Perform any shutdown/cleanup
	return EXIT_SUCCESS;
}

