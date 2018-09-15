#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

	int op;
	int opterr = 0;
	int bufsize = 4096;
	char *buf;
	int i;
	int j;
	int lim;
	int start = 1;
	int arg;
	int fd = 1;
	int temp;
	int done = 0;

int concat(int fd, int fp)
	{
		i = 1;
		while(i != 0)
		{
			//lim = sizeof(buf);
			i = read(fp,buf,bufsize);
			j = write(fd, buf, i);
			//temp = sizeof(buf)/sizeof(buf[0]);
			printf("%d\n", i);
		}
		return 0;
	}



int main(int argc, char **argv)
{
	// Checks the ops and parameters
	while ((op = getopt(argc, argv, "b:o:?")) != -1 && !done)
	{
		switch (op) {
			case 'b':
				start += 2;
				bufsize = atoi(optarg);
				break;
			case 'o':
				start += 2;
				fd = open(optarg, O_CREAT | O_TRUNC | O_APPEND | O_WRONLY, 0666);
				break;
			default:
				done = 1;
				break;
		}
		/*
		if (op == 'b')
		{
			bufsize = atoi(optarg);
			//printf("%d\n", bufsize);
			start += 2;
			//bstatus = 1;
		}
		if (op == 'o')
		{
			//printf("%s\n", optarg);
			start += 2;
			fd = open(optarg, O_CREAT | O_TRUNC | O_APPEND | O_WRONLY, 0666);
		}
		*/
	}
	buf = (char *)malloc(sizeof(char) * bufsize);
	for (arg = start; arg < argc; ++arg)
	{
		if (argv[arg][0] == '-')
			argv[arg]++;
		//printf("Arg %d is %s\n", arg, argv[arg]);
		// inputs[filenum] = argv[arg];
		i = concat(fd, open(argv[arg], O_RDONLY, 0666));
	}
	return 0;

}
