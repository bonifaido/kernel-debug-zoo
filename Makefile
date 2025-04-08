obj-m += debug_zoo.o

CFLAGS += -g

KBUILD=/lib/modules/$(shell uname -r)/build/
 
default:
	$(MAKE) -C $(KBUILD) M=$(PWD) modules

clean:
	$(MAKE) -C $(KBUILD) M=$(PWD) clean

menuconfig:
	$(MAKE) -C $(KBUILD) M=$(PWD) menuconfig

insmod-leak:
	sudo insmod debug_zoo.ko leak=1

insmod-use-after-free:
	sudo insmod debug_zoo.ko use_after_free=1

insmod-lockdep:
	sudo insmod debug_zoo.ko lockdep=1

insmod-race:
	sudo insmod debug_zoo.ko race=1

insmod-slab-poison:
	sudo insmod debug_zoo.ko slab_poison=1

rmmod:
	sudo rmmod debug_zoo

logs:
	sudo dmesg -T --follow | grep -v audit:
