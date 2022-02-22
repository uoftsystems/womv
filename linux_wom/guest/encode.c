//#include <linux/kernel.h>

//#define NULL 0L

#include "encode.h"

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

uint8_t getNextCode(uint8_t data_bits, uint8_t previous_code_bits)
{
	int i;
	
	if (previous_code_bits == 15) {
		return 0xFF;
	}

	for( i = previous_code_bits ; i < 16; i++) {
		if (wom_table[i] == data_bits) {
			return i;
		}	
	}

	printf("could not find data bits %x for prev_code %x\n", data_bits, previous_code_bits);
	return 0xFF;
}

int encode(uint8_t *data_buf, uint8_t *previous_code_buf, uint8_t *code_buf, int data_size)
{
	int i;
	int num_bits;
	num_bits = data_size * 8;
	uint8_t data_byte;
	uint32_t data_byte_index;
	uint8_t data_bit_index;
	uint8_t data_bits;
	uint8_t code_bits, previous_code_bits;
	uint32_t code_size;

	unsigned code_bit_index=0, previous_code_bit_index=0, invalid_code_bits=0;
	unsigned code_byte_index=0, previous_code_byte_index=0;
	uint8_t previous_code_bit, code_bit;
	uint8_t code_byte=0, previous_code_byte = 0;
	uint8_t j=0;
	int code_bits_per_codeword = 4;
	int firstCell = 0;

	// return null if any of the buffers are not initialized.
	if(data_buf == NULL || previous_code_buf == NULL || code_buf == NULL)  {
		return -1;
	}

	code_size = data_size * code_bits_per_codeword / data_bits_per_codeword;

	// make sure caller has sent empty code buffer.
	for(i = 0 ; i < code_size ; i++)
	{
		if(code_buf[i] != 0x00) {
			printf("caller needs to initialize codebuf to 0x00 at offset %d for codesize %d\n", i, code_size);
			assert(0);
		}
	}

	// if firstCell to be programmed has been specified
	// copy all bits from previous_code_buf
	// to code_buf from 0 to firstCell.
	if(firstCell != 0) {
		// byte level copy
		for (i = 0 ; i < (firstCell * code_bits_per_codeword) / 8 ; i++ )
		{
			code_buf[i] = previous_code_buf[i];
		}
		code_bit_index = firstCell * code_bits_per_codeword;
		previous_code_bit_index = firstCell * code_bits_per_codeword;
	}

	// this loop iterates through different data bits. it will recreate data bits
	// if current bits were not programmed.

	printf("data_bits_per_codeword %d num_bits %d\n", data_bits_per_codeword, num_bits);
	for(i = (firstCell * data_bits_per_codeword) ; i < num_bits ; i+= data_bits_per_codeword) {
		// if we have reached the last code bit, break
		if(code_bit_index == code_size * 8) {
			break;
		}
		data_byte_index = i / 8;
		data_bit_index  = i % 8;

		data_byte = data_buf[data_byte_index];
		data_bits = ((data_byte >> (8 - data_bits_per_codeword - data_bit_index)) &  max_data_bit);
		assert(data_bits <= max_data_bit);

		previous_code_bits = 0;
		for (j = 0 ; j < code_bits_per_codeword ; j++)
		{
			previous_code_byte_index = previous_code_bit_index / 8;
			previous_code_byte = previous_code_buf[previous_code_byte_index];
			if(previous_code_byte_index >= code_size) {
				printf("previous_code_byte_index = %d previous_code_bit_index %d\
					code_size = %d code_bit_index = %d\n",
					previous_code_byte_index, previous_code_bit_index, code_size, code_bit_index);
				assert(previous_code_byte_index < code_size);
			}
			previous_code_bit = ((previous_code_byte >> (7 - previous_code_bit_index % 8)) & 0x01);
			previous_code_bits = previous_code_bits | (previous_code_bit << (code_bits_per_codeword - j - 1));
			previous_code_bit_index++;
		}
		assert(previous_code_bits <= 0x0F);
		code_bits = getNextCode(data_bits, previous_code_bits);
		printf("data_bits %x previous_code_bits %x new code bits %x\n", data_bits, previous_code_bits, code_bits);
		
		//if (valid_code_bitmap[code_bit_index / code_bits_per_codeword])  {
			if(code_bits == 0xFF) {
			//	valid_code_bitmap[code_bit_index/ code_bits_per_codeword]=false;
			//	code_bits = previous_code_bits;
				code_bits = 0x0F;
				invalid_code_bits+= code_bits_per_codeword;
				i-=data_bits_per_codeword;
			}
			for (j = 0 ; j < code_bits_per_codeword ; j++)
			{
				code_byte_index = code_bit_index  / 8;
				code_byte = code_buf[code_byte_index];
				code_bit = (code_bits >> (code_bits_per_codeword - j - 1)) & 0x01;
				code_byte = code_byte | (code_bit << (7 - (code_bit_index % 8)));
				code_bit_index++;
				code_buf[code_byte_index]=code_byte;
			}
	}

//	printf("code_bit_index %d invalid_code_bits %d firstCell %d code_bits_per_codeword %d\n", code_bit_index, invalid_code_bits, firstCell, code_bits_per_codeword);
//	return (code_bit_index - invalid_code_bits - (firstCell * code_bits_per_codeword));
	return invalid_code_bits / 8;
}
