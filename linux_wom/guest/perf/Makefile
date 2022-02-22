all: splitn write_to_disk

splitn: splitn.cpp
	g++ --std=c++11 splitn.cpp -o splitn

thread: thread.cpp
	g++ --std=c++11 thread.cpp -lpthread -o thread

write_to_disk: write_to_disk.cpp
	g++ --std=c++11 write_to_disk.cpp -o write_to_disk

clean:
	rm -rf splitn write_to_disk
