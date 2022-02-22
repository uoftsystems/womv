# script to run different workloads on drive.

while read line;
do
	for i in 2500 5000 10000;
	do
	#	echo "$line $i 1600";
		echo "$line $i 1600" >> $line.log
		./get_segments ../wom_traces_realworld/$line $i 1600 >> $line.log
	done
done < input_configs
