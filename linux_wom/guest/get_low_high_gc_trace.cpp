/*
	This program generates a trace that generates a specific amount of invalidated blocks.
	
	low overwrite_pc => high gc, high overwrite pc => low_gc
*/

#include <iostream>
#define llu long long unsigned
#define HIGHGC

using namespace std;

int main(int argc, char *argv[])
{
	llu disk_size = 70000;
	llu segment_size = 100;
	llu overwrite_pc = 10;
	llu write_count = 100;

	int i, j, k;

	llu subtraction_factor;

	subtraction_factor = (segment_size * overwrite_pc) / 100;

	llu segment_counter;
	for(i = 0; i < write_count ; i++)
	{
		#ifndef HIGHGC
		segment_counter=0;
		for(j = 0; j < disk_size ; j++) {
			if(segment_counter == segment_size) {
				segment_counter = 0;
				j -= subtraction_factor;
			}
			cout << j << endl;
			segment_counter++;
		}
		#else
		int total_segment = disk_size / segment_size;
		for(k = 0 ; k < total_segment ; k++) {
			int start = rand() % total_segment;
			for(j = start * segment_size ; j < (start+1) * segment_size ; j++)
			{
				cout << (int)j << endl;
			}
			int start_rand = (rand() % disk_size) - segment_size;
			for(j = 0 ; j < segment_size ; j++)
			{
				int offset = rand() % segment_size;
				cout << (int)(start_rand + offset) << endl;
			}
		}

		#endif
	}
}
