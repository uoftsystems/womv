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

#define ENABLE_RANDOM_BUFFER

#ifdef ENABLE_RANDOM_BUFFER
#define NUM_RAND_BUF 1234
#endif

//#define BLOCK_SIZE 16384
//#define BLOCK_SIZE (1024 * 1024)
#define BLOCK_SIZE 4096

int main(int argc, char *argv[])
{
	int fd, fd_read;
	char buf[BLOCK_SIZE] = {'X'};
	long unsigned c;
	int ret;

	if (argc != 3) {
		printf("Usage: ./write_ones <device> <infile>\n");
		exit(0);
	}

	fd = open(argv[1], O_WRONLY | O_CREAT);
	if (fd < 0) {
		printf("Error opening %s\n", strerror(errno));
		exit(1);
	}

	std::ifstream infile(argv[2]);

  if(infile.fail()) {
		cout << "Error opening I/O file" << endl;
		exit(1);
	}

	long long unsigned bno;

	#ifdef ENABLE_RANDOM_BUFFER
	vector < string > randbuf( NUM_RAND_BUF, string (BLOCK_SIZE, 0));
	cout << "INITIALIZING RANDOM BUF" << endl;
	char seed = 0;

	cout << "Num Buffers " << randbuf.size() << endl;
	cout << "Buffer Size " << randbuf[0].size() << endl;

	for(int i = 0 ; i < randbuf.size() ; i++) {
		for(int j = 0 ; j < randbuf[0].size() ; j++) {
			randbuf[i][j] = seed;
			seed++;
			if (seed >= 127)
				seed = 0;
		}
	}
	cout << "RANDOM BUF INITIALIZED" << endl;
	#endif


	#ifdef ENABLE_RANDOM_BUFFER
	long long unsigned sno = 0;
	#endif

	while (infile >> bno)
	{
		sno++;
		if (sno >= NUM_RAND_BUF) {
			sno = 0;
		}

		printf("BNO=%llu\n", bno);
		off_t o = lseek(fd, BLOCK_SIZE * bno, SEEK_SET);
		if(o < 0) {	
			printf("Seek failed %s\n", strerror(errno));
			exit(1);
		}
		assert(o == bno * BLOCK_SIZE);

		#ifdef ENABLE_RANDOM_BUFFER

			long long unsigned *buf_ptr = (long long unsigned *)randbuf[sno].c_str();
			buf_ptr[0] = bno;
			ret = write(fd, randbuf[sno].c_str(), BLOCK_SIZE);
	
		#elif

			((long long unsigned *)buf)[0] = bno;
			ret = write(fd, buf, BLOCK_SIZE);

		#endif

		if (ret < 0) {
			printf("Error writing file \n");
			exit(1);
		}
	}

	ret = fsync(fd);
	if (ret < 0) {
		printf("Fsync failed\n");
		exit(1);
	}
	ret = close(fd);
	if (ret < 0) {
		printf("fclose failed\n");
		exit(1);
	}
	return 0;
}