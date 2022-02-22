/* 	Update interval of a logical block in a trace is the average number of blocks between two successive updates of the logical block. 
	- two data structures.
	- prev_block_seq. < block_number, previous block sequence>
	- cumulative_block_in_interval map < block_number, num_blocks_between_updates>
	- update_frequency_map <block_number, block_count>
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

#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#define BLOCK_SIZE 4096

void calc_mean_stdev(vector <double> v)
{
	double sum = std::accumulate(v.begin(), v.end(), 0.0);
	double mean = sum / v.size();

	std::vector<double> diff(v.size());
	std::transform(v.begin(), v.end(), diff.begin(),
               std::bind2nd(std::minus<double>(), mean));
	double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
	double stdev = std::sqrt(sq_sum / v.size());
	cout << fixed << "MEAN " << mean << endl;
	cout << fixed << "STDEV " << stdev << endl;
}

/*
	liveliness map is the map of logical block and the number of times it contributes
	to GC.
	liveliness_map [lba] = (current_sno - prev_sno) / disk_size;
*/

int main(int argc, char *argv[])
{
	int fd, fd_read;
	char buf[BLOCK_SIZE] = {0};
	long unsigned c;
	int ret;
	long long unsigned subtrace_size;

	map <int, int> prev_block_seq;
	map <int, int> cumulative_block_in_interval;
	map <int, int> update_frequency_map;
	map <int, int> liveliness_map;
	map <int, int> io_size;

	if (argc != 4) {
		printf("Usage: ./update_interval <infile> <subtrace_size> <subtraces_in_disk>\n");
		printf("subtrace_size: number of pages written to a line\n");
		printf("subtraces_in_disk: number of subtraces that can be accommodated in the drive\n");
		exit(0);
	}

	std::ifstream infile(argv[1]);
	std::ifstream infile2(argv[1]);

	if(infile.fail()) {
		cout << "Error opening I/O file" << endl;
		exit(1);
	}

	long long unsigned bno, original_bno;
	long long unsigned j, k;
	long long unsigned block_serial_id=0;
	long long unsigned real_bno, real_iosize, real_iosize_num_blocks;

	long long unsigned sno=0;
	long long unsigned prev_seq=0;
	long long unsigned distance=0;
	long long unsigned disk_length;
	long long unsigned subtraces_in_disk;

	subtrace_size = atoi(argv[2]);
	subtraces_in_disk = atoi(argv[3]);

	disk_length = subtraces_in_disk * subtrace_size;
	cout << "disk length " << disk_length << endl;
	while (infile >> real_bno >> real_iosize)
	//while (infile >> real_bno)
	{
	//	real_iosize = BLOCK_SIZE;
		real_iosize_num_blocks = real_iosize / BLOCK_SIZE;
		real_bno /= BLOCK_SIZE;
		io_size[real_iosize_num_blocks]++;

		for ( k = real_bno ; k < real_bno + real_iosize_num_blocks ; k++ )
		{
			//cout << k << endl;
			update_frequency_map[k]++;
			prev_seq = prev_block_seq[k];
			distance = sno - prev_seq;
			if(prev_seq == 0) {
				distance = 0;
			}
			cumulative_block_in_interval[k] += distance;
			prev_block_seq[k] = sno;
			sno++;
		}
	}

	// pass 2, once we have update_frequency_map, we look for cold subtraces:

	long long unsigned total_pages = sno;
	sno = 0;
	long long unsigned subtrace_counter=0;
	long long unsigned subtrace_id=0;
	long long unsigned cold_blocks=0;
	long long unsigned cold_subtrace_count=0;
	long long unsigned hot_subtrace_count=0;
	long long unsigned high_gc_subtrace_count=0;
	long long unsigned low_gc_subtrace_count=0;
	long long unsigned unique_update_blocks_in_subtrace=0;

	prev_block_seq.clear();
//	cout << "sub sequence size " << subtrace_size << endl;
//	while (infile2 >> real_bno)
	while (infile2 >> real_bno >> real_iosize)
	{
		//real_iosize = BLOCK_SIZE;
		real_iosize_num_blocks = real_iosize / BLOCK_SIZE;
		real_bno /= BLOCK_SIZE;
		io_size[real_iosize_num_blocks]++;

		for ( k = real_bno ; k < real_bno + real_iosize_num_blocks ; k++ )
		{
			subtrace_counter++;
			subtrace_id = sno / subtrace_size;
			prev_seq = prev_block_seq[k];
			distance = sno - prev_seq;
			if(prev_seq == 0) {
				if(update_frequency_map[k] == 1) {
					distance = total_pages - sno;
				}else{
					distance = 0;
				}
			}else {
				distance = sno - prev_seq;
			}
			prev_block_seq[k] = sno;
			liveliness_map[subtrace_id] += (distance -1 )/ disk_length;

		//	cout << "bno " << k << " sno " << sno << " distance " << distance << endl;

			if(subtrace_counter == subtrace_size) {
		//		cout << "Subtrace ID " << subtrace_id << "Liveliness " << liveliness_map[subtrace_id] << endl;
				// check if there are cold subtraces.
				if(cold_blocks == subtrace_counter) {
					cold_subtrace_count++;
				}
				// check if there are hot subtraces.
				//if(unique_update_blocks_in_subtrace == 0) {
				else if(liveliness_map[subtrace_id] == 0) {
					hot_subtrace_count++;
				} else {
					double x = subtrace_size / liveliness_map[subtrace_id];
					if (x < 2) {
						// > 50% pages not invalidated
						high_gc_subtrace_count++;	
					}else {	// < 50% pages not invalidated
						low_gc_subtrace_count++;
					} 
				}
				subtrace_counter=0;
				cold_blocks = 0;
				unique_update_blocks_in_subtrace = 0;
			} 	
			if(update_frequency_map[k] != 1) {
				update_frequency_map[k]--;
			//	cout << "NOT COLD: " << k << endl;
				cold_blocks = 0;
			} else {
			//	cout << "COLD: " << k << endl;
				cold_blocks++;
				unique_update_blocks_in_subtrace++;
			}
			sno++;
		}
	}
/*
	if(cold_blocks == subtrace_counter) {
		cold_subtrace_count++;
		//cout << cold_subtrace_count << endl;
	}
//	if(unique_update_blocks_in_subtrace == 0) {
	else if(liveliness_map[subtrace_id] == 0) {
		hot_subtrace_count++;
	} else {
	//	double x = subtrace_size / unique_update_blocks_in_subtrace;
		double x = subtrace_size / liveliness_map[subtrace_id];
		if (x < 2) {
			// > 50% pages not invalidated
			high_gc_subtrace_count++;	
		}else {	// < 50% pages not invalidated
			low_gc_subtrace_count++;
		} 
	}
*/
	cout << "Total Subtraces " << sno / subtrace_size << endl;
	cout << "-------------" << endl;
	cout << "Cold Subtrace Count " << cold_subtrace_count << endl;
	cout << "High GC Subtrace Count " << high_gc_subtrace_count << endl;
	cout << "Low GC Subtrace Count " << low_gc_subtrace_count << endl;
	cout << "Hot Subtrace Count " << hot_subtrace_count << endl;

	return 0;
}
