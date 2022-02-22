`generate_trace.sh`

Generates a list of LBAs, partially sequential and partially random that can be used by `write_trace_wom`

EXP1:
`write_trace_wom.cpp`

This code writes from a trace file. the trace file is generated using the `generate_trace.sh` script.

EXP2:
`write_with_db_variations.cpp`

This code writes data from 0 to `max_lba` `N` times with `X` percentage of data variation on each write.

EXP3:
`write_trace_wom.cpp`
We need to change the `WOM_LINEZERO_THRESHOLD` in `drivers/lightnvm/pblk-map.c` file to an appropriate value.

Eg. if the simulated disk capacity is 100K x 4K blocks, and there are 160 lines, each line will have ~7MB capacity.
The threshold should then be set to 520000 (0.5 MB) which is roughly 7\% of the line capacity.

We run `write_trace_wom.cpp` to fill 1/4th of the disk 50 times. we check this in WOM and no_wom modes and
determine the number of GCs that were done (printed in syslog on every line closure).

`Real World Workloads:`

Real world workloads have been automated to run using the script in `femu_wom` repository. 

`femu_wom/autotest`

It is run using the following command:

```
./run-wom-experiment.sh config_file
```

Where config file is a configuration file in the following format:

```
original wom_traces_realworld 316.trace.1608748372231.46067124.trace.W
coding wom_traces_realworld 316.trace.1608748372231.46067124.trace.W
coding_1_4 wom_traces_realworld 316.trace.1608748372231.46067124.trace.W
```

1. The first field is the branch that needs to be checked out
2. The second field tells us whether its a real world workload or a synthetic workload.
This value determines what size of VM will be instantiated, also, which folder does the trace
live inside the VM's `guest` folder.
3. The guest trace is the last field which is in the format `offset_in_bytes,bytes_written`.


