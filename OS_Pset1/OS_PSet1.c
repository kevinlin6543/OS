#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

	int i, j, k, op, lim, arg, fdinput;
	char *buf;
	int bufsize = 4096;
	int fdoutput = 1;
	int done = 0;
	int opterr = 0;

int main(int argc, char **argv)
{
	while ((op = getopt(argc, argv, "b:o:?")) != -1 && !done)
	{
		switch (op)
		{
			case 'b':
				bufsize = atoi(optarg);
				if (bufsize <= 0)
				{
					fprintf(stderr, "Error: Invalid buffer size, %d, chosen\n", bufsize);
					return -1;
				}
				break;
			case 'o':
				fdoutput = open(optarg, O_CREAT | O_TRUNC | O_WRONLY, 0666);
				if (fdoutput < 0)
				{
					fprintf(stderr, "Error: Unable to Create or open file name: %s. %s\n", optarg, strerror(errno));
					return -1;
				}
				break;
			case '?':
				fprintf(stderr, "Error: Invalid arguement modifier: %c\n", optopt);
				return -1;
			default:
				done = 1;
				break;
		}
	}

	buf = (char *)malloc(sizeof(char) * bufsize);

	for (arg = optind; arg < argc || (optind == argc); ++arg)
	{
		i = 1;
		if ((optind == argc) || argv[arg][0] == '-')
			fdinput = 0;
		else
			fdinput = open(argv[arg], O_RDONLY, 0666);
		if (fdinput < 0)
		{
			fprintf(stderr, "Error: Unable to open the file, %s, for reading. %s\n", argv[arg], strerror(errno));
			return -1;
		}
		while(i != 0)
		{
			i = read(fdinput, buf, bufsize);
			if (i < 0)
			{
				fprintf(stderr, "Error: Unable to read file. File Descriptor: %d. File name: %s. %s\n", fdinput, argv[arg], strerror(errno));
				return -1;
			}
			j = write(fdoutput, buf, i);
			while((i != j) && j >= 0)
				j = write(fdoutput, buf+j, i - j);
			if (j < 0)
			{
				fprintf(stderr, "Error: Unable to write to file. File Descriptor: %d. %s\n", fdoutput, strerror(errno));
				return -1;
			}
		}
		if (fdinput != 0)
		{
			k = close(fdinput);
			if (k < 0)
			{
				fprintf(stderr, "Error: Unable to close file, %s. %s\n", argv[arg], strerror(errno));
				return -1;
			}
		}
		if (optind == argc)
			return 0;
	}
	return 0;
}
