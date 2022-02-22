# WOM-v (Voltage Based Write-Once-Memory Codes)

This is the source code repository for our paper titled [Improving the reliability of Next-Generation SSDs using WOM-v codes](https://www.usenix.org/conference/fast22/presentation/jaffer) at the 20th File and Storage Technologies conference (FAST'22).

Please cite our work if it is useful for your research!

```
@inproceedings {277804,
author = {Jaffer, Shehbaz and Mahdaviani, Kaveh and Schroeder, Bianca}
title = {Improving the Reliability of Next Generation {SSDs} using {WOM-v} Codes},
booktitle = {20th USENIX Conference on File and Storage Technologies (FAST 22)},
year = {2022},
isbn = {978-1-939133-26-7},
address = {Santa Clara, CA},
pages = {117--132},
url = {https://www.usenix.org/conference/fast22/presentation/jaffer},
publisher = {USENIX Association},
month = feb,
}

```

# Code Structure

The WOM-v implementation requires two components:

## [FEMU](https://github.com/ucare-uchicago/FEMU)
## [LightNVM](linux_wom/)

# Building Code

- FEMU code can be built using the instructions on the FEMU [website](https://github.com/ucare-uchicago/FEMU)

The default mode for FEMU operation is MLC, we have added support for QLC drive to the FEMU (pushed upstream). The default mode can be changed to add QLC support using the `cell_type` parameter.
More details can be found in our submitted [patch](https://github.com/ucare-uchicago/FEMU/pull/47) 

- Linux LightNVM: 

We recommend building the entire linux kernel provided in the repo that includes our changes in the lightNVM module to add WOM-v implementation.

# Experimental Results

Please refer the paper for more details

# Contact

Shehbaz Jaffer
