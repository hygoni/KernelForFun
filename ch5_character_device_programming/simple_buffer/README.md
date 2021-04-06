# Instruction

if you want to compile module, do
```console
make
```

if you want to install module, do
```console
sudo make install
```

if you want to remove module, do
```console
sudo make uninstall
```

when you installed module, make node for the driver.
you can find major and minor number using dmesg command
```console
mknod /dev/helloworld c MAJOR_NUMBER MINOR_NUMBER
```
