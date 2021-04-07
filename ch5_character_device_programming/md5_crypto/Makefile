NAME = crypto

obj-m += ${NAME}.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=${PWD} modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=${PWD} clean
	rm -f Module.symers modules.order
	rm -f ${NAME}.o ${NAME}.mod ${NAME}.mod.c ${NAME}.mod.o

fclean: clean
	rm -f ${NAME}.ko

re: fclean all

install:
	insmod ${NAME}.ko

uninstall:
	rmmod ${NAME}.ko
