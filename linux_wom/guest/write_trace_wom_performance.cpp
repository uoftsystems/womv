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
#include <limits.h>

using namespace std;

//#define ENABLE_WC_3_4
#define ENABLE_WC_2_4
//#define ENABLE_WC_1_4
//#define ENABLE_WC_NOWOM

#define WOM_2_4 2
#define WOM_1_4 4
#define NOWOM 1

/*
	ENABLE_RANDOM_BUFFER creates a vector of size NUM_RAND_BUF x wom_code.
	The first BUF_SIZE / wom_code of each buffer is filled with random data, the rest
	of the buffer is filled with 0.

	Read block number from infile and write a buffer to that offset.
	The contents of the offset would be  
	
	rand_buf [ offset % NUM_RAND_BUF ]

	The infile (trace_file) is a sequence of LBAs. the WOM LBAs are generated automatically
	by this code.
*/

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
	char buf[BLOCK_SIZE] = {0};
	long unsigned c;
	int ret;

	#ifdef ENABLE_WC_1_4
	int wom_code = WOM_1_4;
	#endif
	#ifdef ENABLE_WC_2_4
	int wom_code = WOM_2_4;
	#endif
	#ifdef ENABLE_WC_3_4
	int wom_code = WOM_3_4;
	#endif
	#ifdef ENABLE_WC_NOWOM
	int wom_code = NOWOM;
	#endif

	if (argc != 3) {
		printf("Usage: ./write_ones <device> <infile>\n");
		exit(0);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		printf("Error opening %s\n", strerror(errno));
		exit(1);
	}

	std::ifstream infile(argv[2]);
	std::ifstream infile2(argv[2]);

	if(infile.fail()) {
		cout << "Error opening I/O file" << endl;
		exit(1);
	}
	cout << "Init " << argv[2] << endl;

	long long unsigned bno, original_bno;
	long long unsigned j, k;
	long long unsigned block_serial_id=0;
	long long unsigned real_bno, real_iosize, real_iosize_num_blocks;

	#ifdef ENABLE_RANDOM_BUFFER
	long long unsigned sno = 0;
	long long unsigned wom_code_sno = 0;
	#endif
	char op;
	int m = 0;
	
	// compute maximum IO Size
//	while(infile >> op >> real_bno >> real_iosize)
	while(infile >> op)
	{
		infile >> real_bno;
//		cout << "bno " << real_bno << " ";
		infile >> real_iosize;
//		cout << "real_iosize " << real_iosize << " ";
//		cout << op << endl;
		if(real_iosize > m) {
			m = real_iosize;
		}
	}
	cout << "MAXIMUM IO SIZE " << m << endl;
	m *= 2;
//	return 0;

	#ifdef ENABLE_RANDOM_BUFFER
	char *randbuf;
	cout << "m " << m << endl;
	cout << "wom_code " << wom_code << endl;
	cout << "blocksize " << BLOCK_SIZE << endl;
	long long unsigned randbuf_size = (m+1) * wom_code;
	cout << "randbuf size" << randbuf_size;
	randbuf = new char[randbuf_size];
//	string randbuf( (m +1) * wom_code * BLOCK_SIZE,'0');
	cout << "INITIALIZING RANDOM BUF" << endl;
	char seed = 0;

//	cout << "Num Buffers " << randbuf.size() << endl;

	int buf_segment_size = BLOCK_SIZE / wom_code;
	for(int i = 0 ; i < randbuf_size ; i++) {
		randbuf[i] = seed;
		seed++;
		if (seed >= 255)
			seed = 0;
	}
	cout << "RANDOM BUF INITIALIZED" << endl;
	#endif

	char read_buf[m+1];

	sno=0;
	while (infile2 >> op)
	{
		infile2 >> real_bno;
		infile2 >> real_iosize;
		real_iosize_num_blocks = real_iosize / BLOCK_SIZE;
		real_iosize_num_blocks *= wom_code;

		//real_bno /= BLOCK_SIZE;
//		real_bno *= wom_code;

//		printf("real_bno = %llu\n", real_bno);
		//printf("real_iosize_num_blocks = %llu\n", real_iosize_num_blocks);

		if(op == 'R') {
			off_t o = lseek(fd, real_bno, SEEK_SET);
			if(o < 0) {
				printf("Seek failed %s\n", strerror(errno));
				exit(1);
			}
			ret = read(fd, read_buf, real_iosize_num_blocks * BLOCK_SIZE);
			if (ret < 0) {
				printf("%s():write failed at %llu\n", __func__, bno);
				exit(1);
			}
	//		assert(ret == real_iosize_num_blocks);
			cout << "sno=" << sno <<  " R " << real_bno << " size=" << real_iosize_num_blocks << endl;
		}else {
			off_t rand_buf_offset = rand() % (randbuf_size - (real_iosize_num_blocks) );
		//	cout << "WRITE "  << BLOCK_SIZE * real_bno << endl;
			off_t o = lseek(fd, real_bno, SEEK_SET);
			if(o < 0) {
				printf("Seek failed %s\n", strerror(errno));
				exit(1);
			}
	//		assert(o == real_bno);
			ret = write(fd, randbuf + rand_buf_offset, real_iosize_num_blocks * BLOCK_SIZE);
			if (ret < 0) {
				printf("%s():write failed at %llu offset %llu error %d\n", __func__, bno, real_iosize_num_blocks, errno);
				exit(1);
			}
			ret = fsync(fd);
			if (ret < 0) {
				printf("%s():fsync failed at %llu\n", __func__, bno);
			}
			cout << "sno=" << sno <<  " W " << real_bno << " size=" << real_iosize_num_blocks << endl;
		}
		sno++;
	}

	ret = close(fd);
	if (ret < 0) {
		printf("fclose failed\n");
		exit(1);
	}
	return 0;
}
