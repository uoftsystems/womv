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
	uint8_t *buf;
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

	if (argc != 2) {
		printf("Usage: ./write_ones <device>\n");
		exit(0);
	}

	fd = open(argv[1], O_WRONLY | O_CREAT);
	if (fd < 0) {
		printf("Error opening %s\n", strerror(errno));
		exit(1);
	}

//	std::ifstream infile(argv[2]);

//	if(infile.fail()) {
//		cout << "Error opening I/O file" << endl;
//		exit(1);
//	}

	int sz = 1024 * 1024 * 1024;
	buf = new uint8_t[sz];
	if(buf == nullptr)
	{
		cout << "Error allocating nullptr" << endl;
		exit(1);
	}

	for(int i = 0 ;i < 16;i++) {
		ret = write(fd, buf , sz);
		if (ret < 0) {
			printf("%s():write failed\n", __func__);
			exit(1);
		}
		assert(ret == sz);
	}
	free(buf);

/*
	#ifdef ENABLE_RANDOM_BUFFER
	vector < string > randbuf( NUM_RAND_BUF * wom_code, string (BLOCK_SIZE, 0));
	cout << "INITIALIZING RANDOM BUF" << endl;
	char seed = 0;

	cout << "Num Buffers " << randbuf.size() << endl;
	cout << "Buffer Size " << randbuf[0].size() << endl;

	int buf_segment_size = BLOCK_SIZE / wom_code;
	for(int i = 0 ; i < randbuf.size() ; i+= wom_code) {
		for(int j = 0 ; j < randbuf[0].size() ; j++) {
			for(int k = 0 ; k < wom_code ; k++) {
				if( (j >= buf_segment_size  * k) && (j <= buf_segment_size * (k+1)) )
				{
					randbuf[i+k][j % buf_segment_size] = seed;
				}
			}
			seed++;
			if (seed >= 127)
				seed = 0;
		}
	}
	cout << "RANDOM BUF INITIALIZED" << endl;
	#endif

	long long unsigned bno, original_bno;
	long long unsigned j;
	long long unsigned block_serial_id=0;

	#ifdef ENABLE_RANDOM_BUFFER
	long long unsigned sno = 0;
	long long unsigned wom_code_sno = 0;
	#endif

	while (infile >> bno)
	{
		#ifdef ENABLE_RANDOM_BUFFER	
		sno++;
		if (sno >= NUM_RAND_BUF) {
			sno = 0;
		}
		#endif

		original_bno = bno;
		for(j = 1; j <= wom_code ; j++) {
			bno = (original_bno * wom_code) + (j - 1);
			printf("BLOCK=%llu ORIGINAL=%llu BNO=%llu\n", block_serial_id++,  original_bno, bno);
			off_t o = lseek(fd, BLOCK_SIZE * bno, SEEK_SET);
			if(o < 0) {
				printf("Seek failed %s\n", strerror(errno));
				exit(1);
			}
			assert(o == bno * BLOCK_SIZE);

		#ifdef ENABLE_RANDOM_BUFFER

			wom_code_sno = (sno * wom_code) + (j - 1);
			if(wom_code_sno >= randbuf.size()) {
				cout << "wom_code_sno = " << wom_code_sno << " randbuf size = " << randbuf.size() << endl;
				assert(wom_code_sno < randbuf.size());
			}
			long long unsigned *buf_ptr = (long long unsigned *)randbuf[wom_code_sno].c_str();
			buf_ptr[0] = original_bno;

			// test
			ret = write(fd, randbuf[rand() % randbuf.size()].c_str(), BLOCK_SIZE);
			if (ret < 0) {
				printf("%s():write failed at %llu\n", __func__, bno);
				exit(1);
			}
			ret = fsync(fd);
			if (ret < 0) {
				printf("%s():fsync failed at %llu\n", __func__, bno);
			}
			
	
		#else

			((long long unsigned *)buf)[0] = original_bno;
			ret = write(fd, buf, BLOCK_SIZE);

		#endif

			if (ret < 0) {
				printf("Error writing file \n");
				exit(1);
			}
		}
	}
*/
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
