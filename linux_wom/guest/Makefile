all: read_blocks write_ones write_zeros write_trace write_to_offset encode_driver write_with_db_variations get_low_high_gc_trace get_unique_blocks write_trace_wom_performance fill_disk

get_unique_blocks: get_unique_blocks.cpp
	g++ get_unique_blocks.cpp -o get_unique_blocks

read_blocks: read_blocks.c
	gcc read_blocks.c -o read_blocks

encode_driver: encode_driver.c encode.c
	gcc -c encode.c -o encode.o 
	gcc encode.o encode_driver.c -o encode_driver

write_ones: write_ones.cpp write_ones_wom.cpp
	g++ write_ones.cpp -o write_ones
	g++ write_ones_wom.cpp -o write_ones_wom

write_zeros: write_zeros.cpp
	g++ write_zeros.cpp -o write_zeros

write_trace: write_trace.cpp write_trace_wom.cpp write_trace_wom_real.cpp
	g++ write_trace.cpp -o write_trace
	g++ write_trace_wom.cpp -o write_trace_wom
	g++ write_trace_wom_real.cpp -o write_trace_wom_real
	
write_to_offset: write_to_offset.c
	g++ write_to_offset.c -o write_to_offset

write_trace_wom_performance: write_trace_wom_performance.cpp
	g++ -g write_trace_wom_performance.cpp -o write_trace_wom_performance

write_with_db_variations: write_with_db_variations.cpp
	g++ -std=c++11 write_with_db_variations.cpp -o write_with_db_variations

get_low_high_gc_trace: get_low_high_gc_trace.cpp
	g++ -std=c++11 get_low_high_gc_trace.cpp -o get_low_high_gc_trace

fill_disk: fill_disk.cpp
	g++ -std=c++11 fill_disk.cpp -o fill_disk

clean:
	rm -rf read_blocks write_ones write_trace write_to_offset write_trace_wom write_ones_wom encode_driver write_zeros write_with_db_variations write_trace_wom_real get_low_high_gc_trace get_unique_blocks write_trace_wom_performance fill_disk
