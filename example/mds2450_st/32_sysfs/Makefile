obj-m := keysample.o
keysample-objs	:= keyint_bottom_kscan_dd_sysfs.o

KDIR	:= /work/kernel/linux-2.6.29-mc100

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	
clean:
	rm -rf keysample_app
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
	rm -rf *.bak
	


