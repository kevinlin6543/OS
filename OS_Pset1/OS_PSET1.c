#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

	char *outfile = NULL;
	int bstatus = 0;
	int ostatus = 0;
	int op;
	int operr;
	int bufsize;
	char buf[4096];
	int i;
	int j;
	int lim;
	int start = 1;
	int arg;
	int fdi;

int concat(int fd, int fp)
	{
		i = 1;
		while(i != 0)
		{
			lim = sizeof(buf);
			i = read(fp,buf,lim);
			j = write(fd, buf, i);
		}
		return 0;
	}



int main(int argc, char **argv)
{
	// Checks the ops and parameters
	while ((op = getopt(argc, argv, "b:o:")) != -1)
	{
		if (op == 'b')
		{
			bufsize = atoi(optarg);
			printf("%d\n", bufsize);
			start += 2;
			bstatus = 1;

		}
		if (op == 'o')
		{
			
			outfile = optarg;
			printf("%s\n", outfile);
			start += 2;
			ostatus = 1;
		}
	}
	if (ostatus = 1)
		fdi = open(outfile, O_CREAT | O_TRUNC | O_APPEND | O_WRONLY, 0666);
	else
		fdi = 1;

	printf("Amount of Files = %d\n", argc-start);
	for (arg = start; arg < argc; ++arg)
	{
		printf("Arg %d is %s\n", arg, argv[arg]);
		// inputs[filenum] = argv[arg];
		i = concat(STDOUT_FILENO, open(argv[arg], O_RDONLY, 0666));

	}
	return 0;

}

/*
int openForWrite(char *path)
{
	return open(path, O_CREAT | O_TRUNC | O_APPEND | O_WRONLY, 0666);
}

int openForRead(char *path)
{
	return open(path, O_RDONLY, 0666);
}
*/
