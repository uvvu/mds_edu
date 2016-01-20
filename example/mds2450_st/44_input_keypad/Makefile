obj-m := keysample.o
keysample-objs	:= keyint_bottom_kscan_platform_input.o

EXTRA_CFLAGS += -I./include

KDIR	:= /work/kernel/linux-2.6.29-mc100


default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	cp -ra keysample.ko /work/filesystem/sys-root/root/
	
clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
	rm -rf *.bak
	rm -rf Module.symvers
	


