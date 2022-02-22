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

//#define BLOCK_SIZE 16384
//#define BLOCK_SIZE (1024 * 1024)
#define BLOCK_SIZE 4096

int main(int argc, char *argv[])
{
	int fd;
	char buf[BLOCK_SIZE] = {0};
	long long unsigned i;
	long unsigned c;
	int ret;
//	int seq_rand = 0;

	fd = open(argv[1], O_WRONLY | O_CREAT);
	if (argc != 3) {
		printf("Usage: ./write_ones <device> <num_blks>\n");
		exit(0);
	}
	
	long long unsigned num_blks = strtol(argv[2], NULL, 10);
//	if (argc == 4) {
//		seq_rand = 1;
//	}

//	int file_size = 262144;
//	int file_size = 1000;

	if (fd < 0) {
		printf("Error opening %s\n", strerror(errno));
		exit(1);
	}
	
//	for (i = 0 ; i < BLOCK_SIZE ; i++) {
//		buf[i] = 1;
//	}

	long long unsigned x;
	for (i = 0; i < num_blks ; i++) {
	//	x = (seq_rand == 0) ? i : rand() % num_blks ;
	//	if(seq_rand) {
	//		printf("i=%llu BNO=%llu\n", i, x);
	//	}else{
	//		printf("BNO=%llu\n", x);
			printf("BNO=%llu\n", i);
	//	}
		off_t o = lseek(fd, BLOCK_SIZE * i, SEEK_SET);
		if(o < 0) {
			printf("Seek failed\n");
			return 1;
		}
		assert(o == i * BLOCK_SIZE);
	//	((long long unsigned *)buf)[0] = x;
		ret = write(fd, buf, BLOCK_SIZE);
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
