sudo nvme lnvm remove -n mydev
sudo rmmod pblk.ko
sudo insmod ../drivers/lightnvm/pblk.ko
sudo nvme lnvm create -d nvme0n1 --lun-begin=0 --lun-end=3 -n mydev -t pblk -f
#sudo dd if=/dev/zero of=/dev/mydev bs=4K count=16
