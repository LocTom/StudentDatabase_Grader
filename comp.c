#include <unistd.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h> 
int main(int argc, char** argv)
{
	int fd1;
	int fd2;
	char a[2]; /* input (output) buffer */
	char b[2]; /* input (output) buffer */
	int charsr1;          /* how many chars were actually read */
	int charsr2;          /* how many chars were actually written */
	int flag = 0;

	fd1 = open(argv[1], O_RDONLY);
	if (fd1 < 0) /* means file open did not take place */
	{
		write(2,"File not found",15);   /* text explaining why */
		exit(-1);
	}

	fd2 = open(argv[2] , O_RDONLY);
	if (fd2 < 0) /* means file open did not take place */
	{
		write(2, "File not found", 15);   /* text explaining why */
		exit(-1);
	}



	while (flag == 0)
	{
		charsr1 = read(fd1, a, 1);
		charsr2 = read(fd2, b, 1);
		
		if (charsr1 == -1 || charsr2 == -1)
		{
			write(2, "Couldnt read charcter", 22);   /* text explaining why */
			exit(-1);
		}
		else if (charsr1 == 0 && charsr2 == 0)
		{
			flag = 2;
		}
		else if (charsr1 == 0 || charsr2 == 0)
		{
			flag = 1;
		}
		else if (a[0] != b[0])
		{
			
			flag = 1;
		}
		

	}


	close(fd1);		/* free allocated structures */
	close(fd2);		/* free allocated structures */

	return flag;


}
