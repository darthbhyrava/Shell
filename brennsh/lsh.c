#include<stdio.h>

//Function to read the command
#define *LSH_RL_BUFSIZE 1024
char *lsh_read_line()
{
	int bufsize = LSH_RL_BUFSIZE;
	int position = 0;	
	char *buffer = malloc(sizeof(char)*bufsize);
	int c;

	if(!buffer) {
		fprintf(stderr, "lsh:allocation error!\n");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		c=getchar();
		if ( c==EOF || c=='\n')
		{
			buffer[position]='\0';
			return buffer;
		} 
		else
		{
			buffer[position]=c;
		}
		position++;

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

