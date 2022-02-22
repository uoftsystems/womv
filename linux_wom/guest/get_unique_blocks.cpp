#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

#define BLOCK_SIZE 4096

int main(int argc, char *argv[])
{
	int fd, fd_read;
	char buf[BLOCK_SIZE] = {0};
	long unsigned c;
	int ret;
	long long unsigned real_bno, real_iosize, i;

	map <long long unsigned, long long unsigned> M;

	if (argc != 2) {
		printf("Usage: ./write_ones <infile>\n");
		exit(0);
	}

	fd = open(argv[1], O_WRONLY | O_CREAT);
	if (fd < 0) {
		printf("Error opening %s\n", strerror(errno));
		exit(1);
	}

	std::ifstream infile(argv[1]);

	if(infile.fail()) {
		cout << "Error opening I/O file" << endl;
		exit(1);
	}

	while (infile >> real_bno >> real_iosize)
	{
		for(i = 0 ; i < real_iosize ; i+= BLOCK_SIZE) {
			M[real_bno + i]++;
		}
	}

	cout << M.size() << endl;
	return 0;
}
