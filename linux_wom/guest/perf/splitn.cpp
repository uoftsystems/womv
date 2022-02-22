/*
	This program takes a parameter input file and a parameter K
	and divides input file into K files in /tmp.
*/

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

using namespace std;

#define BLOCK_SIZE 4096

int main(int argc, char *argv[])
{
	int fd, fd_read;
	char buf[BLOCK_SIZE] = {0};
	int ret;
	int i;
	int k;

	if (argc != 3) {
		printf("Usage: ./splitn <infile> <K>\n");
		exit(0);
	}

	std::ifstream infile(argv[1]);
	if(infile.fail()) {
		cout << "Error opening I/O file" << endl;
		exit(1);
	}

	k = atoi(argv[2]);

	ofstream *of = new ofstream[100];
	
	string s(argv[1]);
	for(i = 0 ; i < k; i++)
	{
		of[i].open((s+ to_string(i)).c_str(), std::ofstream::trunc);
	}

	char c;
	long long unsigned st, sz;
	int idx=0;

	while(infile >> c >> st >> sz)
	{
		of[idx] << c;
		of[idx] << " ";
		of[idx] << st;
		of[idx] << " ";
		of[idx] << sz;
		of[idx] << endl;
		idx = (idx + 1) % k;		
	}

	return 0;
}
