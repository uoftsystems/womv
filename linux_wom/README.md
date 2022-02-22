This codebase contains code for Linux LightNVM Module.

The following should also be installed for pblk target management:

`https://github.com/linux-nvme/nvme-cli`

The micro-benchmarking scripts are placed in `guest/benchmarks` folder.

README in `guest/benchmarks` folder contains details for each of the experiments.

`guest/encode_driver` is the test script for checking encode operation.

Currently the following modes are supported:

`ENABLE_NO_WOM`
`ENABLE_WOM_2_4`
`ENABLE_WOM_1_4`

The following needs to be modified to enable a particular mode:

```
drivers/lightnvm/pblk-write.c
drivers/lightnvm/encode.h
```

The LBA address space gets reduced by a factor of 2 for `WOM_2_4` and by a factor of
4 for `WOM_1_4`. Any workload generated and run on top of the device should be between
0 and `MAX_LBA` size.
