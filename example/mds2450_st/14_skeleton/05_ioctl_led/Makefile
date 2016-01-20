obj-m   := sk.o

KDIR    := /work/kernel-mds2450-3.0.22

all:
	make -C $(KDIR) SUBDIRS=$(PWD) modules
	arm-unknown-linux-gnueabi-gcc sk_app.c -o sk_app

clean:	
	make -C $(KDIR) SUBDIRS=$(PWD) clean
