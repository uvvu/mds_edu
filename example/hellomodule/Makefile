
KERNEL := /work/kernel-mds2450-3.0.22

obj-m := hello_driver.o

all:
	make -C $(KERNEL) SUBDIRS=$(PWD) modules
clean:
	make -C $(KERNEL) SUBDIRS=$(PWD) clean
