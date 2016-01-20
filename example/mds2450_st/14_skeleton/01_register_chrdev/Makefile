obj-m   := sk.o

#KDIR    := /lib/modules/$(shell uname -r)/build
#KDIR    := /work/linux-2.6.29-mc100
KDIR := /work/kernel-mds2450-3.0.22


all:
	make -C $(KDIR) SUBDIRS=$(PWD) modules

clean:	
	make -C $(KDIR) SUBDIRS=$(PWD) clean
