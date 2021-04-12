ADDER = adder
NAME = helloworld

obj-m += ${ADDER}.o
obj-m += ${NAME}.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f Module.symvers modules.order
	rm -f ${NAME}.o ${NAME}.mod ${NAME}.mod.c ${NAME}.mod.o

fclean: clean
	rm -f ${NAME}.ko

re: fclean all

# requires root privilege
install:
	insmod ${NAME}.ko

uninstall:
	rmmod ${NAME}.ko
