#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>


#define BLOCK_SIZE 4096

typedef unsigned long long ticks;

static __inline__ ticks getticks(void)
{
     unsigned a, d;
     asm("cpuid");
     asm volatile("rdtsc" : "=a" (a), "=d" (d));

     return (((ticks)a) | (((ticks)d) << 32));
}

int count_bits(char *buf)
{
	int i, j;
	char c;
	int count =0;

	for (i = 0 ; i < BLOCK_SIZE ; i++) {
		c = buf[i];
		for (j = 0 ; j < 8 ; j++) {
			if (c & (1 << j)) {
				count +=  1;
			}
//			printf("j=%d res=%d\n", j , c & (1 << j));
		}
//	exit(1);
	}
	return count;
}

int main(int argc, char *argv[])
{
	int fd;
	char buf[4096] = {'X'};
	long unsigned c;
	int j;

	if (argc != 3) {
		printf("./read_blocks <device> <num_blks>\n");
		exit(0);
	}
	
	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		printf("Error opening %s\n", strerror(errno));
		exit(1);
	}

	int r = 0;
	ticks tick,tick1,tickh;
	unsigned long long time =0;
	
	tick = getticks();

	int num_blks = strtol(argv[2], NULL, 10);

//	while (read (fd, buf, BLOCK_SIZE) > 0) {
	for (int i = 0 ; i < num_blks; i++) {
		printf("%d\n", i);
		off_t o = lseek(fd, BLOCK_SIZE * i, SEEK_SET);
		if(o < 0) {	
			printf("Seek failed\n");
			return 1;
		}
		assert(o == i * BLOCK_SIZE);
		read (fd, buf, BLOCK_SIZE);
//		if( ((int *)buf)[0] != i) {
		for(j = 0 ; j < 4096 ; j++) {
			if( buf[j] != 0) {
				printf("Mismatch at %d offset %d %d\n", i, j, (int)buf[j]);
			}else {
				printf("MATCH\n");
			}
		}
	}
//	for (int i = 0 ; i < num_blks ; i ++) {
//		printf("reading block %d\n", i);
//	 	int ret = read (fd, buf, BLOCK_SIZE);
//		if (ret < 0) {
//			printf("error reading %s\n", strerror(errno));
//			exit(1);
//		}
//	}
//		c += count_bits(buf);

	tick1 = getticks();

	time = (unsigned long long)((tick1-tick)/2904595);
	printf("\ntime in MS  = %llu\n",time);

//	printf("Count %lu\n", c);
}
