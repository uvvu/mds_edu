obj-m += hello.o
#obj-m += hello_param.o
hello-objs := start.o stop.o

KDIR	:= /work/kernel-mds2450-3.0.22

all:
	make -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean

