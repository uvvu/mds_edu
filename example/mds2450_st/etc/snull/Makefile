#obj-m += snull.o
#obj-m += 01_snull-register_netdev.o
obj-m += 02_snull-open.o 

KDIR	:= /work/kernel/linux-2.6.29-mc100
#KDIR	:= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
