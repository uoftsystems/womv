#ifndef ENCODE_H
#define ENCODE_H

//#include <stdio.h>
//#include <stdint.h>
//#include <stdbool.h>
//#include <assert.h>

//#define BLOCK_SIZE 4096

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

#define ENABLE_WC_2_4
//#define ENABLE_WC_1_4

#define WOM_1_4 1
#define WOM_2_4 2
#define WOM_3_4 3

uint8_t no_wom [4][4] = {
				{0x00 ,0x00,0xFF,0xFF },
				{0x01 ,0x01,0xFF,0xFF },
				{0x02 ,0x02,0xFF,0xFF },
				{0x03 ,0x03,0xFF,0xFF }
			};

uint8_t wom_2_3 [4][4] = { 
				{0x00, 0x07,0xFF,0xFF},
				{0x04, 0x03,0xFF,0xFF},
				{0x02, 0x05,0xFF,0xFF},
				{0x01, 0x06,0xFF,0xFF}
			};

uint8_t wom_2_4_code1 [4][4] = { 
				{0x00, 0x08, 0x0F,0xFF},
				{0x04, 0x0C, 0x0B,0xFF},
				{0x02, 0x0A, 0x0D,0xFF},
				{0x01, 0x09, 0x0E,0xFF}
			};

uint8_t wom_2_4_code2 [4][4] = { 
				{0x00, 0x04, 0x07, 0x0B},
				{0x01, 0x05, 0x08, 0x0C},
				{0x02, 0x06, 0x09, 0x0D},
				{0x03, 0x07, 0x0A, 0x0E}
			};

uint8_t wom_2_4_gen5[16] = { 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3 };
uint8_t wom_1_4_gen15[16] = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };
uint8_t wom_3_4_gen2[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7 };

#ifdef ENABLE_WC_2_4
	int code_type = WOM_2_4;
	uint8_t *wom_table = wom_2_4_gen5;
	int data_bits_per_codeword = 2;
	int max_data_bit = 0x03;
	int code_bits_per_dataword = 2;
#elif ENABLE_WC_1_4
	int code_type = WOM_1_4;
	uint8_t *wom_table = wom_1_4_gen15;
//	int data_size = 1;
	int data_bits_per_codeword = 1;
	int max_data_bit = 0x01;
	int code_bits_per_dataword = 4;
#elif ENABLE_WC_3_4
	int code_type = WOM_3_4;
	uint8_t *wom_table = wom_3_4_gen2;
	int data_bits_per_codeword = 3;	
	int max_data_bit = 0x07;
#endif

int code_bits_per_codeword = 4;

int encode(uint8_t *data_buf, uint8_t *previous_code_buf, uint8_t *code_buf, int sz);

#endif
