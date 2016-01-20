obj-m := keysample.o
keysample-objs	:= keyint_bottom_kscan_dd_platform_class.o
keysample-objs	:= keyint_bottom_kscan_dd_platform.o
#keysample-objs	:= keyint_bottom_kscan_platform.o

EXTRA_CFLAGS += -I./include

KDIR	:= /work/kernel/linux-2.6.29-mc100


default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	
clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
	rm -rf *.bak
	


