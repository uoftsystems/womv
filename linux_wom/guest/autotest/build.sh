coding_scheme=$1
workload_type=$2
workload_name=$3
workload=$workload_type/$workload_name
ts=$4

echo "IN BUILD BRANCH $coding_scheme WORKLOAD $workload TIMESTAMP $ts"

cd 
cd ~/linux_wom
git checkout $coding_scheme
make M=drivers/lightnvm
