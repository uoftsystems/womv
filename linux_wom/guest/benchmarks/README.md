Benchmark Details:

1. Change in Data Buffer Contents.

2. Number of cells invalidated after multiple write cycles.

For this benchmark, we count the total number of cells that become invalidated while programming each page.
We maintain this count for each line in the variable `wom_total_invalid_cells`.
The count of invalid cells is returned after every encode operation.
We reset this value each time a new line is opened for write.
We print this value each time a line is closed after a write.

1 iteration of write is 1 write of 0 to maximum possible LBAs. (`MAX_LBA`)

seq (update 100%)
	no_wom, wom_code_1_4, wom_code_2_4, wom_code_3_4
random update (75% of pages updated randomly `MAX_LBA` lbas, low GC as ~25% valid pages have to be rewritten)
	no_wom, wom_code_1_4, wom_code_2_4, wom_code_3_4
random update (50% of pages updated randomly `MAX_LBA` lbas, mid GC as ~50% valid pages have to be rewritten)
	no_wom, wom_code_1_4, wom_code_2_4, wom_code_3_4
update (25% of pages updated randomly `MAX_LBA` lbas, high GC as ~75% of valid pages have to be re-written)
	no_wom, wom_code_1_4, wom_code_2_4, wom_code_3_4

each of these LBAs are spread across `MAX_LBA` range.

relevant script:

`get_invalid_cells.sh`

This script takes a syslog containing multiple "CLOSE\_LINE" messages in syslog along with multiple lines 
and the first "update lba" number for a random I/O workload. The "update lba" line is used to mark where 
the previous Write Cycle ended and a new Write Cycle began.
The output of this script is the number of invalidated cells in each Write Cycle.

3. Number of reduced EC cycles.

4. Performance Tradeoff.

5. Write Amplification Factor.

6. Effect of locality on WOM Code Gains.
