if [[ $# -ne 2 ]]; then
	echo "Usage run_multiple.sh <k> <infile>";
	echo "infile is the prefix of all input files";
	exit;
fi

k=$1
infile=$2

for i in `seq 0 $(($k-1))`;
do
#	cat vmautopass | sudo -S ./write_to_disk /dev/mydev /tmp/out$i &
	nohup cat vmautopass | sudo -S ./perf/write_to_disk /dev/mydev $2$i &
#	echo $i;
done
