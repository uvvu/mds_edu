obj-m += kerneltimer.o

KDIR	:= /work/linux-2.6.29-mc100

all:
	make -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
