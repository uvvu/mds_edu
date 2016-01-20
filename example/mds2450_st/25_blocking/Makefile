obj-m := blocking.o

KDIR	:= /work/kernel-mds2450-3.0.22

all:
	make -C $(KDIR) SUBDIRS=$(PWD) modules
	arm-unknown-linux-gnueabi-gcc -o blocking_app blocking_app.c

clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
	rm -rf blocking_app

