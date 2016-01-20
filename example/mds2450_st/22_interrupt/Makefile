obj-m := keysample.o
keysample-objs	:= keyint_base.o
#keysample-objs	:= keyint_bottom.o

EXTRA_CFLAGS += -I./include

KDIR 	:= /work/kernel-mds2450-3.0.22


default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

