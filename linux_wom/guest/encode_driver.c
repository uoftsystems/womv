#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

//#define ENABLE_WC_1_4
//#define ENABLE_WC_2_4
#define ENABLE_WC_3_4
#define BLOCK_SIZE 4096

/*
	00 -> 0x00
	01 -> 0x55
	10 -> 0xAA
	11 -> 0xFF

*/

// helper functions:

// fill_buf (buf, content, bytes)

// check_buf (buf, char, bytes)


extern int encode(uint8_t *data_buf, uint8_t *previous_code_buf, uint8_t *code_buf, int sz);

void fill_buf(uint8_t *buf, uint8_t c, int bytes)
{
	int i;

	for(i = 0 ; i < bytes; i++) {
		buf[i] = c;
	}
	for(i = bytes; i < BLOCK_SIZE; i++) {
		buf[i] = 0x00;	
	}
}

bool check_buf(uint8_t *buf, uint8_t c, int bytes)
{
	int i;

	for(i = 0;  i < bytes; i++) {
		if(buf[i] != c) {
			printf("At IDX %d expected %x found %x\n", i, c, buf[i]);	
			return false;
		}
	}

	return true;
}

void print_buf(uint8_t *buf, int bytes)
{
	int i;

	for(i = 0 ; i < bytes; i++) {
		printf("%x ", buf[i]);
	}
	printf("\n");
}

void test_enable_wc_3_4(uint8_t *data_buf, uint8_t *prev_code_buf, uint8_t *code_buf)
{
	int ret, r;
	int i;
/*
	uint8_t next_code = 0x11;
	fill_buf(prev_code_buf, 0x00, BLOCK_SIZE);
	for ( i = 0 ; i < 15 ; i++) {
		if(i % 2 == 0) {
			fill_buf(data_buf, 0xFF, BLOCK_SIZE / 4);
		} else {
			fill_buf(data_buf, 0x00, BLOCK_SIZE / 4);
		}
		fill_buf(code_buf, 0x00, BLOCK_SIZE);
		ret = encode(data_buf, prev_code_buf, code_buf, BLOCK_SIZE / 4);
		r = check_buf(code_buf, next_code, BLOCK_SIZE);
		assert(r == true);
		assert(ret == 0);
		next_code+= 0x11;
		memcpy(prev_code_buf, code_buf, BLOCK_SIZE);
	}
*/
}

void test_enable_wc_1_4(uint8_t *data_buf, uint8_t *prev_code_buf, uint8_t *code_buf)
{
	int ret, r;
	int i;

	uint8_t next_code = 0x11;
	fill_buf(prev_code_buf, 0x00, BLOCK_SIZE);
	for ( i = 0 ; i < 15 ; i++) {
		if(i % 2 == 0) {
			fill_buf(data_buf, 0xFF, BLOCK_SIZE / 4);
		} else {
			fill_buf(data_buf, 0x00, BLOCK_SIZE / 4);
		}
		fill_buf(code_buf, 0x00, BLOCK_SIZE);
		ret = encode(data_buf, prev_code_buf, code_buf, BLOCK_SIZE / 4);
		r = check_buf(code_buf, next_code, BLOCK_SIZE);
		assert(r == true);
		assert(ret == 0);
		next_code+= 0x11;
		memcpy(prev_code_buf, code_buf, BLOCK_SIZE);
	}

}

void test_enable_wc_2_4(uint8_t *data_buf, uint8_t *prev_code_buf, uint8_t *code_buf)
{
	int ret;
	bool r;

	// GEN 1 (0000, 01) --> 0001
	fill_buf(prev_code_buf, 0x00, BLOCK_SIZE);
	fill_buf(data_buf, 0x55, BLOCK_SIZE / 2);

	ret = encode(data_buf, prev_code_buf, code_buf, BLOCK_SIZE / 2);
	r = check_buf(code_buf, 0x11, BLOCK_SIZE);
	assert(r == true);
	assert(ret == 0);

	// GEN 2 (0001, 00) --> 0100
	memcpy(prev_code_buf, code_buf, BLOCK_SIZE);
	fill_buf(data_buf, 0x00, BLOCK_SIZE / 2);
	fill_buf(code_buf, 0x00, BLOCK_SIZE);

	ret = encode(data_buf, prev_code_buf, code_buf, BLOCK_SIZE / 2);
	r = check_buf(code_buf, 0x44, BLOCK_SIZE);
	assert(r == true);
	assert(ret == 0);

	// GEN 2 (rewrite same data) --> 0100
	// GEN 2 (0100, 00) --> 0100

	memcpy(prev_code_buf, code_buf, BLOCK_SIZE);
	fill_buf(code_buf, 0x00, BLOCK_SIZE);
	ret = encode(data_buf, prev_code_buf, code_buf, BLOCK_SIZE / 2);
	r = check_buf(code_buf, 0x44, BLOCK_SIZE);
	assert(r == true);
	assert(ret == 0);

	// SAME GEN Transition to higher layer
	// 0100, 01 -> 0101

	memcpy(prev_code_buf, code_buf, BLOCK_SIZE);
	fill_buf(code_buf, 0x00, BLOCK_SIZE);
	fill_buf(data_buf, 0x55, BLOCK_SIZE / 2);
	ret = encode(data_buf, prev_code_buf, code_buf, BLOCK_SIZE / 2);
	r = check_buf(code_buf, 0x55, BLOCK_SIZE);
	assert(r == true);
	assert(ret == 0);

	// 0101, 11 -> 0x77

	memcpy(prev_code_buf, code_buf, BLOCK_SIZE);
	fill_buf(code_buf, 0x00, BLOCK_SIZE);
	fill_buf(data_buf, 0xFF, BLOCK_SIZE / 2);
	ret = encode(data_buf, prev_code_buf, code_buf, BLOCK_SIZE / 2);
	r = check_buf(code_buf, 0x77, BLOCK_SIZE);
	assert(r == true);
	assert(ret == 0);

	// 0x77 , 10 -> 1010 (A)

	memcpy(prev_code_buf, code_buf, BLOCK_SIZE);
	fill_buf(code_buf, 0x00, BLOCK_SIZE);
	fill_buf(data_buf, 0xAA, BLOCK_SIZE / 2);
	ret = encode(data_buf, prev_code_buf, code_buf, BLOCK_SIZE / 2);
	r = check_buf(code_buf, 0xAA, BLOCK_SIZE);
	assert(r == true);
	assert(ret == 0);

	// 0xAA , 01 -> 1101 (D)

	memcpy(prev_code_buf, code_buf, BLOCK_SIZE);
	fill_buf(code_buf, 0x00, BLOCK_SIZE);
	fill_buf(data_buf, 0x55, BLOCK_SIZE / 2);
	ret = encode(data_buf, prev_code_buf, code_buf, BLOCK_SIZE / 2);
	r = check_buf(code_buf, 0xDD, BLOCK_SIZE);
	assert(r == true);
	assert(ret == 0);

	// 0xDD , 00 -> EINVAL

	memcpy(prev_code_buf, code_buf, BLOCK_SIZE);
	fill_buf(code_buf, 0x00, BLOCK_SIZE);
	fill_buf(data_buf, 0x00, BLOCK_SIZE / 2);
	ret = encode(data_buf, prev_code_buf, code_buf, BLOCK_SIZE / 4);
	printf("ret = %d\n", ret);
	r = check_buf(code_buf, 0xFF, BLOCK_SIZE / 2);
	assert(r == true);
	printf("First Block Size bits are safe\n");
//	r = check_buf(code_buf + (BLOCK_SIZE / 2), 0xDD, BLOCK_SIZE / 4);
//	assert(r == true);
	assert(ret == BLOCK_SIZE / 2);

	// prev data buffer
	int i;
	for(i = 0 ; i < BLOCK_SIZE ; i++) {
		prev_code_buf[i] = 0xDD;
	}
	prev_code_buf[0] = 0xFF;
	prev_code_buf[BLOCK_SIZE - 1] = 0xFF;

	fill_buf(data_buf, 0xFF, BLOCK_SIZE / 2);
	fill_buf(code_buf, 0x00, BLOCK_SIZE);

	ret = encode(data_buf, prev_code_buf, code_buf, BLOCK_SIZE / 2);
	printf("ret = %d\n", ret);
	r = check_buf(code_buf, 0xFF, BLOCK_SIZE);
	assert(r == true);

}

int main(int argc, char *argv[])
{
	uint8_t data_buf[BLOCK_SIZE] = {0x00};
	uint8_t prev_code_buf[BLOCK_SIZE] = {0x00};
	uint8_t code_buf[BLOCK_SIZE] = {0x00};

	// data buf-> 01, prev_code_buf-> 00, new code buf -> 0001 (0x11)

	#ifdef ENABLE_WC_2_4
		test_enable_wc_2_4 (data_buf, prev_code_buf, code_buf);
	#endif
	#ifdef ENABLE_WC_1_4
		printf("enable _wc 14 \n");
		test_enable_wc_1_4 (data_buf, prev_code_buf, code_buf);
	#endif
	#ifdef ENABLE_WC_3_4
		test_enable_wc_3_4 (data_buf, prev_code_buf, code_buf);
	#endif
	
	return 0;
}
