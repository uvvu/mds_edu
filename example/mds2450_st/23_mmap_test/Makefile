obj-m	:= mmapcall_dev.o

KDIR  := /work/kernel-mds2450-3.0.22
#KDIR	:= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) SUBDIRS=$(PWD) miodules
	arm-unknown-linux-gnueabi-gcc mmapcall_app.c -o mmapcall_app
	arm-unknown-linux-gnueabi-gcc mmapcall_dev.c -o mmapcall_dev

clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
