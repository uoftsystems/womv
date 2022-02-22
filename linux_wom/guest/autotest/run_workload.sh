coding_scheme=$1
workload_type=$2
workload=$3
ts=$4

cd
cd linux_wom/guest/
make clean
make

res_dir=result-${ts}-${coding_scheme}-${workload_type}-${workload}
mkdir $res_dir

#./test_target.sh
# logging

echo "Enable Logging"

sudo tail -f /var/log/kern.log > $res_dir/log &

sleep 5

echo "RUN: $workload_type/$workload"
if [ "$workload_type" = "wom_traces_synthetic" ]; then
	echo "Run Synthetic Trace"
	sudo ./write_trace_wom /dev/mydev $workload_type/$workload
elif [ "$workload_type" = "wom_traces_dbchange" ]; then
	echo "Run DBChange Trace"
	sudo ./write_with_db_variations /dev/mydev 70000 100 $workload $coding_scheme
elif [ "$workload_type" = "wom_traces_perf" ]; then
	echo "Run For Performance"
	echo "Fill DISK"
#	sudo ./write_trace_wom /dev/mydev $workload_type/${coding_scheme}_filldisk
	sudo ./fill_disk /dev/mydev
	echo "FILLED DISK FOR $coding_scheme $workload"
	sleep 10
	echo "Run Real Trace"
	mkdir $workload_type/$workload/res_$coding_scheme
	make -C perf
	./perf/splitn $workload_type/$workload 5
	./perf/run_multiple.sh 5 $workload_type/$workload
#	sudo ./write_trace_wom_performance /dev/mydev $workload_type/$workload
else
	echo "Fill DISK"
	sudo ./write_trace_wom /dev/mydev $workload_type/${coding_scheme}_filldisk
	echo "FILLED DISK FOR $coding_scheme $workload"
	sleep 10
	echo "Run Real Trace"
	sudo ./write_trace_wom_real /dev/mydev $workload_type/$workload
fi

echo "Workload Run Complete"

sleep 5
