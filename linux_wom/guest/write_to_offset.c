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
	char buf[BLOCK_SIZE] = {'X'};
	long unsigned c;
	int ret;

	fd = open(argv[1], O_WRONLY | O_CREAT);
	if (argc != 3) {
		printf("Usage: ./write_ones <device> offset_block\n");
		exit(0);
	}
	
	long long unsigned blk_no = strtol(argv[2], NULL, 10);

//	int file_size = 262144;
//	int file_size = 1000;

	if (fd < 0) {
		printf("Error opening %s\n", strerror(errno));
		exit(1);
	}
	
//	for (i = 0 ; i < BLOCK_SIZE ; i++) {
//		buf[i] = 1;
//	}

//	for (i = 0; i < num_blks ; i++) {
		printf("BNO=%llu\n", blk_no);
		off_t o = lseek(fd, BLOCK_SIZE * blk_no, SEEK_SET);
		if(o < 0) {	
			printf("Seek failed\n");
			return 1;
		}
		assert(o == blk_no * BLOCK_SIZE);
		((long long unsigned *)buf)[0] = blk_no;
		ret = write(fd, buf, BLOCK_SIZE);
		if (ret < 0) {
			printf("Error writing file \n");
			exit(1);
		}	
//	}

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
