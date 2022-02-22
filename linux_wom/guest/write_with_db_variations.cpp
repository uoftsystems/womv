/*
	# experiment 2 DATA VARIATION.

	This code writes data to the underlying disk starting with a random buffer BUF.
	the disk is written N times, in each write, the disk buffer is changed by X\%.

	Write all blocks sequentially.
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
#include <set>
#include <cstdint>
#include <iostream>

using namespace std;

//#define BLOCK_SIZE 16384
//#define BLOCK_SIZE (1024 * 1024)
#define BLOCK_SIZE 4096

#define ENABLE_WOM_2_4

#define NO_WOM 1
#define WOM_2_4 2
#define WOM_1_4 4

void change_buf(uint8_t *c, long long unsigned pc, int wom_type)
{
	int change_size = (pc * BLOCK_SIZE / wom_type) / 100;
	int i;
	set <int> S;

	while (S.size() < change_size) {
		S.insert(rand() % (BLOCK_SIZE / wom_type ));
	}

	for (auto s: S) {
//		cout << "Before " << (int)c[s] << endl;
		c[s] = ~c[s];
//		cout << "After " << (int)c[s] << endl;
	}
}

int main(int argc, char *argv[])
{
	int fd;
	uint8_t buf[BLOCK_SIZE] = {0};
	long long unsigned i, t, seq = 0, k;
	long unsigned c;
	int ret;
	int wom_type;

	fd = open(argv[1], O_WRONLY | O_CREAT);
	if (argc != 6) {
		printf("Usage: ./write_with_db_variations <device> <num_blks> <num_times> <pc> <womtype>\n");
		exit(0);
	}
	
	long long unsigned num_blks  	= strtol(argv[2], NULL, 10);
	long long unsigned num_times 	= strtol(argv[3], NULL, 10);
	long long unsigned pc 		= strtol(argv[4], NULL, 10);
	string coding_scheme(argv[5]);

	if(coding_scheme == "original") {
		wom_type = NO_WOM;
	}else if(coding_scheme == "coding") {
		wom_type = WOM_2_4;
	}else {
		wom_type = WOM_1_4;
	}

	if (fd < 0) {
		printf("Error opening %s\n", strerror(errno));
		exit(1);
	}
	
	for (i = 0 ; i < BLOCK_SIZE / wom_type ; i++) {
		buf[i] = i % 256;
	}

	for(t = 0 ; t < num_times ; t++) {
		for (i = 0; i < num_blks ; i++) {
			for (k = 0; k < wom_type ; k++) {
				long long unsigned new_offset = i * wom_type + k;
				printf("SEQ %llu BNO=%llu\n", seq, new_offset);
				off_t o = lseek(fd, BLOCK_SIZE * new_offset, SEEK_SET);
				if(o < 0) {
					printf("Seek failed\n");
					return 1;
				}
				assert(o == new_offset * BLOCK_SIZE);
				ret = write(fd, buf, BLOCK_SIZE);
				if (ret < 0) {
					printf("Error writing file \n");
					exit(1);
				}

				ret = fsync(fd);
				if (ret < 0) {
					printf("Fsync failed at %llu bno %llu\n", seq, new_offset);
					exit(1);
				}
			seq++;
			}
		}
		change_buf(buf, pc, wom_type);
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
