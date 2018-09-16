#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

	int i, j, k, op, lim, arg, temp, fdinput;
	char *buf;
	int bufsize = 4096;
	int start = 1;
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
				start += 2;
				bufsize = atoi(optarg);
				if (bufsize <= 0)
				{
					fprintf(stderr, "Invalid buffer size, %d, chosen. Must be greater than 0", bufsize);
					return -1;
				}
				break;
			case 'o':
				start += 2;
				fdoutput = open(optarg, O_CREAT | O_TRUNC | O_APPEND | O_WRONLY, 0666);
				if (fdoutput < 0)
				{
					fprintf(stderr, "Unable to Create or open file name: %s. %s\n", optarg, strerror(errno));
					return -1;
				}
				break;
			default:
				done = 1;
				break;
		}
	}

	buf = (char *)malloc(sizeof(char) * bufsize);

	for (arg = start; arg < argc; ++arg)
	{
		i = 1;
		if (argv[arg][0] == '-')
			argv[arg]++;

		fdinput = open(argv[arg], O_RDONLY, 0666);

		if (fdinput < 0)
		{
			fprintf(stderr, "Unable to open the file, %s, for reading. %s\n", argv[arg], strerror(errno));
			return -1;
		}
		else
		{
			while(i != 0)
			{
				i = read(fdinput, buf, bufsize);
				if (i < 0)
				{
					fprintf(stderr, "Unable to read file. File Descriptor: %d. File name: %s. %s\n", fdinput, argv[arg], strerror(errno));
					return -1;
				}
				j = write(fdoutput, buf, i);
				if (j < 0)
				{
					fprintf(stderr, "Unable to write to file. File Descriptor: %d. %s\n", fdoutput, strerror(errno));
					return -1;
				}
			}
			k = close(fdinput);
			if (k < 0)
			{
				fprintf(stderr, "Unable to close file, %s. %s\n", argv[arg], strerror(errno));
				return -1;
				
			}
		}
	}
	return 0;
}
