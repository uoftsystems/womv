
#This script extracts line numbers that have the word update in them.
#The line numbers can be used to derive where a new WC started.
#
#eg. if the trace has 25% blocks being accessed, then after every 4
#updates of a particular block number, 1 WC will be over.

# INPUTS:

# File: A file containing lines grepped from syslog. The lines have one of the
# two expressions:
# a. CLOSE
# b. update LBA
# where LBA is the first random I/O block number that is written to the disk.

# eg for a LBA run of 250000 blocks, in the I/O file, the random I/O block would
# be first block after 249999 in the input I/O file.

if [[ $# -ne 3 ]]; then
	echo "Usage: ./extract_update_lines.sh <FILE_with_CLOSED_and_UPDATE_LBA> <seq_no> <num_updates_per_rand_wc>"
	exit
fi

file=$1

#cat seq_1_rand5.closed | grep -n update | cut -d":" -f1
cat $file | grep -n update | cut -d":" -f1 | tee line_nos

#echo "USE"
#echo "sed -n 'X,Yp' $file"

sno=1
wc=0

seq_write=$2
num_updates_per_rand_wc=$3


start_line=1
while read line;
do
	echo "Processing $line"
	if [[ $seq_write -ne 0 ]]; then
		end_line=$(($line-1))
		#sed -n '$start_line,$end_linep' $file > wc.$wc
		sed -n "$start_line,$end_line p" $file > wc.$wc
		echo "SEQ IO $wc -> $start_line, $end_line"
		seq_write=$(($seq_write-1))
		start_line=$line
		wc=$(($wc+1))
	else
		if [[ $(($sno % $num_updates_per_rand_wc)) -eq 0 ]]; then
			end_line=$(($line-1))
			sed -n "$start_line,$end_line p" $file > wc.$wc
			echo "RAND IO $wc -> $start_line, $end_line"
			start_line=$line
			wc=$(($wc+1))
		fi
		sno=$(($sno+1))
	fi
done < line_nos

wc_total=$(($wc-1))
for i in `seq 0 $wc_total`;
do
	invalid_cells=`rev wc.$i | cut -d" " -f1 | rev | awk '{ sum += $1 } END { printf "%.1f", sum }'`
#inv=$(printf "%d" $invalid_cells)
	echo $i $invalid_cells
done
