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

void calc_mean_stdev(vector <double> v, double no, bool divide)
{
	double sum = std::accumulate(v.begin(), v.end(), 0.0);
	double mean = sum / v.size();

	std::vector<double> diff(v.size());
	std::transform(v.begin(), v.end(), diff.begin(),
               std::bind2nd(std::minus<double>(), mean));
	double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
	double stdev = std::sqrt(sq_sum / v.size());
	if (divide) {
		cout << fixed << "MEAN " << mean/no << endl;
		cout << fixed << "STDEV " << stdev/no << endl;
	} else {
		cout << fixed << "MEAN " << mean << endl;
		cout << fixed << "STDEV " << stdev << endl;
	}
}

int main(int argc, char *argv[])
{
	int fd, fd_read;
	char buf[BLOCK_SIZE] = {0};
	long unsigned c;
	int ret;

	map <int, int> prev_block_seq;
	map <int, int> cumulative_block_in_interval;
	map <int, int> distance_from_end;
	map <int, int> update_frequency_map;
	map <int, int> io_size;

	if (argc != 2) {
		printf("Usage: ./update_interval <infile>\n");
		exit(0);
	}

	std::ifstream infile(argv[1]);

	if(infile.fail()) {
		cout << "Error opening I/O file" << endl;
		exit(1);
	}

	long long unsigned bno, original_bno;
	long long unsigned j, k;
	long long unsigned block_serial_id=0;
	long long unsigned real_bno, real_iosize, real_iosize_num_blocks;

	long long unsigned sno=0;
	long long unsigned prev_seq;
	long long unsigned distance;

	//while (infile >> real_bno >> real_iosize)
	while (infile >> real_bno)
	{
		real_iosize=BLOCK_SIZE;
		real_iosize_num_blocks = real_iosize / BLOCK_SIZE;
//		real_bno /= BLOCK_SIZE;
		io_size[real_iosize_num_blocks]++;

		for ( k = real_bno ; k < real_bno + real_iosize_num_blocks ; k++ )
		{
			update_frequency_map[k]++;
			prev_seq = prev_block_seq[k];
			distance = sno - prev_seq;
			if(prev_seq == 0) {
				distance = 0;
				distance_from_end[k] = sno;
			} else {
				distance_from_end.erase(k);
			}
			cumulative_block_in_interval[k] += distance;
			prev_block_seq[k] = sno;
			sno++;
		}
	}

	long long unsigned cumulative_distance;
	long long unsigned num_instances;
	vector <double > v;

	cout << "Cumulative Block " << endl;
	for(auto it : cumulative_block_in_interval) {
		real_bno = it.first;
		cumulative_distance = it.second;
		num_instances = update_frequency_map[real_bno];
		if (cumulative_distance !=0) {
//			cout << real_bno << " " << cumulative_distance / num_instances << endl;
//			uifile << real_bno << " " << cumulative_distance / num_instances << endl;
			for(j = 0 ; j < num_instances ; j++)
				v.push_back(cumulative_distance / num_instances);
		}
	}
/*
	for(auto it : distance_from_end) {
		cumulative_distance = it.second;
		v.push_back(sno - cumulative_distance);
	}
*/

	cout << "update freq map size " << update_frequency_map.size() << endl;

	calc_mean_stdev(v, update_frequency_map.size(), true);
	v.clear();

	cout << "Update Frequency Map " << endl;
	long long unsigned freq;
	for(auto it : update_frequency_map) {
		real_bno = it.first;
		freq = it.second;

//		cout << real_bno << " " << freq << endl;
//		uffile << real_bno << " " << freq << endl;
		v.push_back(freq);
	}
	calc_mean_stdev(v, sno, false);
	v.clear();

	cout << "IO Size " << endl;
	long long unsigned sz;
	long long unsigned count;
	long long unsigned i;
	for(auto it : io_size) {
		sz = it.first;
		count = it.second;

		for(i = 0 ; i < count;i++)
			v.push_back(sz);

//		cout << sz << " " << count << endl;
//		sfile << sz << " " << count << endl;
	}
	calc_mean_stdev(v, sno, false);
	v.clear();

//	uifile.close();
//	uffile.close();
//	sfile.close();
	return 0;
}
